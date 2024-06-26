#include <HostTests.h>
#define ARDUINOJSON_USE_LONG_LONG 1
#include <JsonObjectStream6.h>
#include <Data/CStringArray.h>
#include <Data/CString.h>
#include <FlashString/Stream.hpp>

class JsonTest6 : public TestGroup
{
public:
	JsonTest6() : TestGroup(_F("ArduinoJson6"))
	{
	}

	void execute() override
	{
		DEFINE_FSTR_LOCAL(flashString1, "FlashString-1");
		DEFINE_FSTR_LOCAL(flashString2, "FlashString-2");
		DEFINE_FSTR_LOCAL(formatStrings, "Compact\0Pretty\0MessagePack");
		DEFINE_FSTR_LOCAL(test_json, "test.json");
		DEFINE_FSTR_LOCAL(test_msgpack, "test.msgpack");

		doc["string1"] = "string value 1";
		auto json = doc.as<JsonObject>();
		DEFINE_FSTR_LOCAL(FS_number2, "number2");
		json[FS_number2] = 12345;
		auto arr = doc.createNestedArray("arr");
		arr.add(flashString1);
		doc[flashString2] = flashString1;

		TEST_CASE("serialize")
		{
			DEFINE_FSTR_LOCAL(serialized1,
							  "{\"string1\":\"string value "
							  "1\",\"number2\":12345,\"arr\":[\"FlashString-1\"],\"FlashString-2\":\"FlashString-1\"}");

			String s = Json::serialize(doc);
			debug_d("Test doc: %s", s.c_str());
			REQUIRE(s == serialized1);
			CString cs;
			Json::serialize(doc, cs);
			debug_d("Test doc: %s", cs.c_str());
			REQUIRE(cs == serialized1);
		}

		TEST_CASE("Json::measure()")
		{
			CStringArray formats(formatStrings);
			const uint8_t sizes[] = {100, 132, 82};
			for(auto fmt = Json::Compact; fmt <= Json::MessagePack; ++fmt) {
				auto len = Json::measure(doc, fmt);
				debug_d("Measure(doc, %s) = %u", formats[fmt], len);
				REQUIRE(len == sizes[fmt]);
			}
		}

		TEST_CASE("Json::getValue(doc[\"number2\"], value))")
		{
			int value;
			if(Json::getValue(doc["number2"], value)) {
				debug_d("number2 = %d", value);
				REQUIRE(value == 12345);
			} else {
				debug_e("number2 not found");
			}
		}

		TEST_CASE("Json::getValue(doc[\"string\"], value))")
		{
			String value;
			REQUIRE(Json::getValue(doc["string"], value) == false);
		}

		TEST_CASE("Json::getValue(doc[\"arr\"][1], value")
		{
			String value;
			REQUIRE(Json::getValue(doc["arr"][0], value) == true);
			debug_d("arr = %s", value.c_str());
			REQUIRE(value == flashString1);
		}

		// Keep a reference copy for when doc gets messed up
		auto sourceDoc = doc;

		TEST_CASE("Json::serialize(doc, String), then save to file")
		{
			DEFINE_FSTR_LOCAL(expectedOutput,
							  "{\"string1\":\"string value "
							  "1\",\"number2\":12345,\"arr\":[\"FlashString-1\"],\"FlashString-2\":\"FlashString-1\"}")
			String s;
			auto sz = Json::serialize(doc, s);
			REQUIRE_EQ(sz, 100U);
			REQUIRE(expectedOutput == s);
			REQUIRE(fileSetContent(test_json, s) == int(s.length()));
		}

		TEST_CASE("Json::saveToFile(doc, test_json, Json::Pretty)")
		{
			REQUIRE(Json::saveToFile(doc, test_json, Json::Pretty) == true);
		}

		TEST_CASE("Json::loadFromFile(doc, test_json)")
		{
			REQUIRE(Json::loadFromFile(doc, test_json) == true);
			String s = fileGetContent(test_json);
			REQUIRE(s.length() == 132);
			debug_d("%s", s.c_str(), s.length());
		}

		TEST_CASE("Json::serialize(doc, MemoryDataStream*)")
		{
			doc = sourceDoc;
			auto stream = new MemoryDataStream;
			REQUIRE(Json::serialize(doc, stream, Json::Compact) == 100);
			auto avail = stream->available();
			debug_d("serialize -> %d bytes", avail);
			REQUIRE(avail == 100);
			REQUIRE(Json::deserialize(doc, stream, Json::Compact) == true);
			auto measured = Json::measure(doc);
			debug_d("deserialize -> %u bytes", measured);
			REQUIRE(measured == size_t(avail));
			Serial.println(Json::serialize(doc));
			delete stream;
		}

		TEST_CASE("nullptr checks")
		{
			MemoryDataStream* stream = nullptr;
			auto count = Json::serialize(doc, stream);
			debug_d("Json::serialize(stream = nullptr) = %u", count);
			REQUIRE(count == 0);
			REQUIRE(Json::deserialize(doc, stream) == false);
			debug_d("doc.memoryUsage = %u", doc.memoryUsage());
		}

		TEST_CASE("CString serialisation")
		{
			doc = sourceDoc;
			CString cs;
			Json::serialize(doc, cs);
			debug_hex(DBG, "serialized", cs.c_str(), cs.length());
			REQUIRE(cs.length() == 100);
			REQUIRE(Json::deserialize(doc, cs) == true);
			debug_d("doc.memoryUsage = %u", doc.memoryUsage());
		}

		String serialised;
		TEST_CASE("String serialisation")
		{
			doc = sourceDoc;
			serialised = Json::serialize(doc);
			debug_hex(DBG, "serialized", serialised.c_str(), serialised.length());
			REQUIRE(serialised.length() == 100);
			REQUIRE(Json::deserialize(doc, serialised) == true);
			debug_d("doc.memoryUsage = %u", doc.memoryUsage());
		}

		char buffer[256];
		TEST_CASE("Buffer serialisation")
		{
			doc = sourceDoc;
			size_t len = Json::serialize(doc, buffer);
			debug_hex(DBG, "Serialised", buffer, len);
			REQUIRE(len == serialised.length());
			REQUIRE(memcmp(buffer, serialised.c_str(), len) == 0);
		}

		TEST_CASE("Buffer/Size serialisation")
		{
			doc = sourceDoc;
			auto len = Json::serialize(doc, buffer, sizeof(buffer));
			debug_hex(DBG, "Serialised", buffer, len);
			REQUIRE(len == serialised.length());
			REQUIRE(memcmp(buffer, serialised.c_str(), len) == 0);
			REQUIRE(Json::deserialize(doc, buffer, len) == true);
			debug_hex(DBG, "De-serialized", buffer, len);
			debug_d("doc.memoryUsage = %u", doc.memoryUsage());
		}

		TEST_CASE("Json::saveToFile(doc, test_msgpack, Json::MessagePack)")
		{
			REQUIRE(Json::saveToFile(doc, test_msgpack, Json::MessagePack) == true);
		}

		TEST_CASE("Json::loadFromFile(doc, test_msgpack, Json::MessagePack)")
		{
			REQUIRE(Json::loadFromFile(doc, test_msgpack, Json::MessagePack) == true);
			String s = fileGetContent(test_msgpack);
			debug_hex(DBG, "MSG", s.c_str(), s.length());
			REQUIRE(s.length() == 82);
		}

		TEST_CASE("Json::serialize(doc, Serial)")
		{
			Json::serialize(doc, Serial);
			Serial.println();
			Json::serialize(doc, Serial, Json::Pretty);
			Serial.println();
		}

		TEST_CASE("Json::measure(doc2)")
		{
			StaticJsonDocument<10> doc2;
			auto measured = Json::measure(doc2);
			debug_d("measure(doc2) = %u", measured);
			REQUIRE(measured == 4);
			String s;
			Json::serialize(doc2, s);
			debug_hex(DBG, "doc2", s.c_str(), s.length());
			REQUIRE(s == "null");
		}

		DEFINE_FSTR_LOCAL(jsonTest, "{\"key1\":\"value1\",\"key2\":\"value2\"}");

		TEST_CASE("Serialise to MemoryDataStream")
		{
			Json::deserialize(doc, jsonTest);
			auto stream = new MemoryDataStream;
			stream->setTimeout(0);
			size_t serializedLength = Json::serialize(doc, stream);
			debug_d("serialized length = %u", serializedLength);
			REQUIRE(serializedLength == jsonTest.length());
			String content;
			REQUIRE(stream->moveString(content));
			debug_d("stream->read returned %u", content.length());
			debug_d("%s", content.c_str(), content.length());
			REQUIRE(content == jsonTest);
			delete stream;
		}

		TEST_CASE("De-serialise from MemoryDataStream")
		{
			auto stream = new MemoryDataStream;
			stream->print(jsonTest);
			REQUIRE(Json::deserialize(doc, stream) == true);
			String s;
			serializeJson(doc, s);
			REQUIRE(s == jsonTest);
			delete stream;
		}

		TEST_CASE("LONGLONG support")
		{
			StaticJsonDocument<256> doc;
			JsonObject root = doc.to<JsonObject>();
			const uint64_t testnum = 0x12345678ABCDEF99ULL;
			root["longtest"] = testnum;
			REQUIRE(root["longtest"] == testnum);
		}

		TEST_CASE("FSTR comparison")
		{
			doc[FS_number2] = FS_number2;
			REQUIRE(doc[FS_number2] == FS_number2);
		}

		/*
		 * Dangling reference https://github.com/bblanchon/ArduinoJson/issues/1120
		 * Fixed in ArduinoJson 6.13.0
		 */
		TEST_CASE("ArduinoJson #1120")
		{
			StaticJsonDocument<500> doc;
			constexpr char str[] = "{\"contents\":[{\"module\":\"Packet\"},{\"module\":\"Analog\"}]}";
			deserializeJson(doc, str);

			auto value = doc[F("contents")][1];
			debug_i("value.isNull() = %u", value.isNull());
			REQUIRE(value.isNull() == false);
			REQUIRE(value[_F("module")] == FS("Analog"));
		}
		TEST_CASE("Speed checks")
		{
			speedChecks();
		}
	}

	void __noinline loadBuffer(const char* buffer, size_t length)
	{
		TEST_ASSERT(Json::deserialize(doc, buffer, length));
	}

	void __noinline loadFlashString()
	{
		TEST_ASSERT(Json::deserialize(doc, Resource::test_json));
	}

	void __noinline loadFlashStringViaStream(bool useFlashRead)
	{
		FSTR::Stream stream(Resource::test_json, useFlashRead);
		TEST_ASSERT(Json::deserialize(doc, stream));
	}

	void __noinline loadStream(IDataSourceStream& stream)
	{
		stream.seekFrom(0, SeekOrigin::Start);
		TEST_ASSERT(Json::deserialize(doc, stream));
	}

	void __noinline loadFile(const char* filename)
	{
		FileStream fs(filename);
		loadStream(fs);
	}

	template <typename T> void check(T func)
	{
		for(unsigned i = 0; i < 4; ++i) {
			ElapseTimer timer;
			func();
			debug_i("Time: %s", timer.elapsedTime().toString().c_str());
		}
	}

	void speedChecks()
	{
		PSTR_ARRAY(filename, "test.json");
		if(!fileExist(filename)) {
			FileStream fs(filename, File::CreateNewAlways | File::WriteOnly);
			REQUIRE(fs.isValid());
			FSTR::Stream os(Resource::test_json);
			REQUIRE(fs.copyFrom(&os, os.available()) == Resource::test_json.length());
		}

		TEST_CASE("loadBuffer")
		{
			LOAD_FSTR(buffer, Resource::test_json);
			check([&] { loadBuffer(buffer, Resource::test_json.length()); });
		}
		TEST_CASE("loadFlashString")
		{
			check([&] { loadFlashString(); });
		}
		TEST_CASE("loadFlashString via Stream (cached)")
		{
			check([&] { loadFlashStringViaStream(false); });
		}
		TEST_CASE("loadFlashString via Stream (un-cached)")
		{
			check([&] { loadFlashStringViaStream(true); });
		}
		TEST_CASE("loadFile")
		{
			check([&] { loadFile(filename); });
		}
		TEST_CASE("loadStream")
		{
			FileStream fs(filename);
			check([&]() { loadStream(fs); });
		}
	}

private:
	StaticJsonDocument<2048> doc;
};

void REGISTER_TEST(ArduinoJson6)
{
	registerGroup<JsonTest6>();
}
