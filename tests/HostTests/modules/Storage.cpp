#include <HostTests.h>
#include <Storage.h>
#include <Nvs/PartitionManager.hpp>

#ifdef ENABLE_NVS_ENCRYPTION
#include "mbedtls/aes.h"
#endif

DEFINE_FSTR_LOCAL(partNvsTest, "nvs_test")

#define REQUIRE_HANDLE(expr)                                                                                           \
	do {                                                                                                               \
		auto& handle_ref = expr;                                                                                       \
		REQUIRE_EQ(ESP_OK, nvs_errno);                                                                                 \
		REQUIRE2(handle_ref, expr);                                                                                    \
	} while(0)
#define TEST_HANDLE_ERR(res, expr)                                                                                     \
	do {                                                                                                               \
		auto& handle_ref = expr;                                                                                       \
		REQUIRE_EQ(res, nvs_errno);                                                                                    \
		REQUIRE2(!handle_ref, expr);                                                                                   \
	} while(0)

class TestDevice : public Storage::Device
{
public:
	String getName() const override
	{
		return F("testDevice");
	}

	size_t getBlockSize() const override
	{
		return sizeof(uint32_t);
	}

	size_t getSize() const override
	{
		return 0x40000000;
	}

	Type getType() const override
	{
		return Type::unknown;
	}

	bool read(uint32_t address, void* dst, size_t size) override
	{
		for(unsigned i = 0; i < size; ++i) {
			static_cast<uint8_t*>(dst)[i] = address + i;
		}
		return true;
	}

	bool write(uint32_t address, const void* src, size_t size) override
	{
		return false;
	}

	bool erase_range(uint32_t address, size_t size) override
	{
		return false;
	}
};

class PartitionTest : public TestGroup
{
public:
	PartitionTest() : TestGroup(_F("Partition"))
	{
	}

	void execute() override
	{
		auto dev = new TestDevice;
		Storage::registerDevice(dev);

		listPartitions();

		delete dev;

		nvs_test();
	}

	void listPartitions()
	{
		for(auto it = Storage::findPartition(); it; ++it) {
			auto part = *it;

			Serial.println();
			Serial.print(_F("Device '"));
			Serial.print(part.getDeviceName());
			Serial.print(_F("', partition '"));
			Serial.print(part.name());
			Serial.print(_F("', type "));
			Serial.print(int(part.type()));
			Serial.print('/');
			Serial.print(int(part.subType()));
			Serial.print(" (");
			Serial.print(part.longTypeString());
			Serial.print(_F("), address 0x"));
			Serial.print(part.address(), HEX);
			Serial.print(_F(", size 0x"));
			Serial.print(part.size(), HEX);
			Serial.println();

			testRead(part, 0xE0, 0x20, true);
			testRead(part, 10, 20, true);
			testRead(part, part.size() - 10, 10, true);
			testRead(part, part.size() - 10, 11, false);
		}
	}

	void testRead(Storage::Partition& part, uint32_t address, size_t size, bool shouldSucceed)
	{
		auto buf = new uint8_t[size];
		bool success = part.read(address, buf, size);
		if(success) {
			m_printHex("READ", buf, size, address);
			REQUIRE(shouldSucceed == true);
		} else {
			debug_e("read(0x%08x, %u) failed", address, size);
			REQUIRE(shouldSucceed == false);
		}
		delete[] buf;
	}

	bool nvsErasePartition()
	{
		if(!nvs::closeContainer(partNvsTest)) {
			return false;
		}
		auto part = nvs::partitionManager.findPartition(partNvsTest);
		if(!part) {
			return false;
		}
		return part.erase_range(0, part.size());
	}

	bool nvsInit(bool canErase)
	{
		if(nvs::openContainer(partNvsTest)) {
			return true;
		}

		if(!canErase) {
			return false;
		}

		if(nvs_errno != ESP_ERR_NVS_NO_FREE_PAGES && nvs_errno != ESP_ERR_NVS_NEW_VERSION_FOUND) {
			return false;
		}

		debug_w("nvsInit failed (0x%x), erasing partition and retrying", nvs_errno);

		if(!nvsErasePartition()) {
			return false;
		}

		return nvs::openContainer(partNvsTest);
	}

	bool nvsDeinit()
	{
		return nvs::closeContainer(partNvsTest);
	}

	nvs::HandlePtr nvsOpenHandle(const String& nsName, nvs::OpenMode openMode)
	{
		return nvs::openHandle(partNvsTest, nsName, openMode);
	}

	void nvs_test()
	{
		auto initialFreeHeap = system_get_free_heap_size();

		auto printHeapUsage = [&]() { debug_i("Heap used: %u", initialFreeHeap - system_get_free_heap_size()); };

		TEST_CASE("Partition name no longer than 16 characters")
		{
			DEFINE_FSTR_LOCAL(TOO_LONG_NAME, "0123456789abcdefg");

			REQUIRE(!nvs::openContainer(TOO_LONG_NAME));
			REQUIRE_EQ(nvs_errno, ESP_ERR_INVALID_ARG);

			REQUIRE(nvs::closeContainer(TOO_LONG_NAME));
		}

		TEST_CASE("flash erase deinitializes initialized partition")
		{
			REQUIRE(nvsErasePartition());
			REQUIRE(nvsInit(false));

			nvs::HandlePtr handle;
			REQUIRE_HANDLE(handle = nvsOpenHandle("uninit_ns", NVS_READWRITE));
			printHeapUsage();
			handle.reset();

			REQUIRE(nvsErasePartition());

			// expected: no partition is initialized since nvsErasePartition() deinitialized the partition again
			TEST_HANDLE_ERR(ESP_ERR_NVS_NOT_INITIALIZED, handle = nvsOpenHandle("uninit_ns", NVS_READWRITE));

			// just to be sure it's deinitialized in case of error and not affecting other tests
			REQUIRE(nvsDeinit());
		}

		TEST_CASE("various nvs tests")
		{
			REQUIRE(nvsInit(true));

			nvs::HandlePtr handle1;

			TEST_HANDLE_ERR(ESP_ERR_NVS_NOT_FOUND, handle1 = nvsOpenHandle("test_namespace1", NVS_READONLY));

			REQUIRE_HANDLE(handle1 = nvsOpenHandle("test_namespace2", NVS_READWRITE));
			REQUIRE(handle1->eraseAll());
			REQUIRE(handle1->setItem("foo", 0x12345678));
			REQUIRE(handle1->setItem("foo", 0x23456789));

			nvs::HandlePtr handle2;
			REQUIRE_HANDLE(handle2 = nvsOpenHandle("test_namespace3", NVS_READWRITE));
			REQUIRE(handle2->eraseAll());
			REQUIRE(handle2->setItem("foo", 0x3456789a));
			DEFINE_FSTR_LOCAL(str, "value 0123456789abcdef0123456789abcdef");
			REQUIRE(handle2->setString("key", str));

			int32_t v1;
			REQUIRE(handle1->getItem("foo", v1));
			REQUIRE_EQ(0x23456789, v1);

			int32_t v2;
			REQUIRE(handle2->getItem("foo", v2));
			REQUIRE_EQ(0x3456789a, v2);

			REQUIRE(handle2->getString("key") == str);

			printHeapUsage();

			handle1.reset();
			handle2.reset();

			REQUIRE(nvsDeinit());
		}

		TEST_CASE("calculate used and free space")
		{
			nvs_stats_t stat1;
			nvs_stats_t stat2;

			REQUIRE(nvsInit(true));

			// erase if have any namespace
			auto container = nvs::partitionManager.lookupContainer(partNvsTest);
			REQUIRE(container != nullptr);
			REQUIRE(container->fillStats(stat1));
			if(stat1.namespaceCount != 0) {
				REQUIRE(nvsDeinit());
				REQUIRE(nvsErasePartition());
				REQUIRE(nvsInit(false));
				container = nvs::partitionManager.lookupContainer(partNvsTest);
				REQUIRE(container != nullptr);
			}

			// after erase. empty partition
			REQUIRE(container->fillStats(stat1));
			REQUIRE(stat1.freeEntries != 0);
			REQUIRE(stat1.namespaceCount == 0);
			REQUIRE(stat1.totalEntries != 0);
			REQUIRE(stat1.usedEntries == 0);

			// create namespace test_k1
			nvs::HandlePtr handle1;
			REQUIRE_HANDLE(handle1 = nvsOpenHandle("test_k1", NVS_READWRITE));
			REQUIRE(container->fillStats(stat2));
			REQUIRE_EQ(stat2.freeEntries + 1, stat1.freeEntries);
			REQUIRE_EQ(stat2.namespaceCount, 1U);
			REQUIRE_EQ(stat2.totalEntries, stat1.totalEntries);
			REQUIRE_EQ(stat2.usedEntries, 1U);

			// create pair key-value com
			REQUIRE(handle1->setItem("com", 0x12345678));
			REQUIRE(container->fillStats(stat1));
			REQUIRE_EQ(stat1.freeEntries + 1, stat2.freeEntries);
			REQUIRE_EQ(stat1.namespaceCount, 1U);
			REQUIRE_EQ(stat1.totalEntries, stat2.totalEntries);
			REQUIRE_EQ(stat1.usedEntries, 2U);

			// change value in com
			REQUIRE(handle1->setItem("com", 0x01234567));
			REQUIRE(container->fillStats(stat2));
			REQUIRE_EQ(stat2.freeEntries, stat1.freeEntries);
			REQUIRE_EQ(stat2.namespaceCount, 1U);
			REQUIRE_NEQ(stat2.totalEntries, 0U);
			REQUIRE_EQ(stat2.usedEntries, 2U);

			// create pair key-value ru
			REQUIRE(handle1->setItem("ru", 0x00FF00FF));
			REQUIRE(container->fillStats(stat1));
			REQUIRE_EQ(stat1.freeEntries + 1, stat2.freeEntries);
			REQUIRE_EQ(stat1.namespaceCount, 1U);
			REQUIRE_NEQ(stat1.totalEntries, 0U);
			REQUIRE_EQ(stat1.usedEntries, 3U);

			// amount valid pair in namespace 1
			size_t h1_count_entries;
			REQUIRE(handle1->getUsedEntryCount(h1_count_entries));
			REQUIRE_EQ(h1_count_entries, 2U);

			// create namespace test_k2
			nvs::HandlePtr handle2;
			REQUIRE_HANDLE(handle2 = nvsOpenHandle("test_k2", NVS_READWRITE));
			REQUIRE(container->fillStats(stat2));
			REQUIRE_EQ(stat2.freeEntries + 1, stat1.freeEntries);
			REQUIRE_EQ(stat2.namespaceCount, 2U);
			REQUIRE_EQ(stat2.totalEntries, stat1.totalEntries);
			REQUIRE_EQ(stat2.usedEntries, 4U);

			// create pair key-value
			REQUIRE(handle2->setItem("su1", 0x00000001));
			REQUIRE(handle2->setItem("su2", 0x00000002));
			REQUIRE(handle2->setItem("sus", 0x00000003));
			REQUIRE(container->fillStats(stat1));
			REQUIRE_EQ(stat1.freeEntries + 3, stat2.freeEntries);
			REQUIRE_EQ(stat1.namespaceCount, 2U);
			REQUIRE_EQ(stat1.totalEntries, stat2.totalEntries);
			REQUIRE_EQ(stat1.usedEntries, 7U);

			REQUIRE_EQ(stat1.totalEntries, stat1.usedEntries + stat1.freeEntries);

			// amount valid pair in namespace 2
			size_t h2_count_entries;
			REQUIRE(handle2->getUsedEntryCount(h2_count_entries));
			REQUIRE_EQ(h2_count_entries, 3U);

			REQUIRE_EQ(stat1.usedEntries, h1_count_entries + h2_count_entries + stat1.namespaceCount);

			printHeapUsage();

			handle1.reset();
			handle2.reset();

			nvs::HandlePtr handle3;
			// create namespace test_k3
			REQUIRE_HANDLE(handle3 = nvsOpenHandle("test_k3", NVS_READWRITE));
			REQUIRE(container->fillStats(stat2));
			REQUIRE_EQ(stat2.freeEntries + 1, stat1.freeEntries);
			REQUIRE_EQ(stat2.namespaceCount, 3U);
			REQUIRE_EQ(stat2.totalEntries, stat1.totalEntries);
			REQUIRE_EQ(stat2.usedEntries, 8U);

			// create pair blobs
			uint32_t blob[12];
			REQUIRE(handle3->setBlob("bl1", &blob, sizeof(blob)));
			REQUIRE(container->fillStats(stat1));
			REQUIRE_EQ(stat1.freeEntries + 4, stat2.freeEntries);
			REQUIRE_EQ(stat1.namespaceCount, 3U);
			REQUIRE_EQ(stat1.totalEntries, stat2.totalEntries);
			REQUIRE_EQ(stat1.usedEntries, 12U);

			// amount valid pair in namespace 2
			size_t h3CountEntries;
			REQUIRE(handle3->getUsedEntryCount(h3CountEntries));
			REQUIRE_EQ(h3CountEntries, 4U);

			REQUIRE_EQ(stat1.usedEntries, h1_count_entries + h2_count_entries + h3CountEntries + stat1.namespaceCount);

			printHeapUsage();

			handle3.reset();

			REQUIRE(nvsDeinit());
			REQUIRE(nvsErasePartition());

			printHeapUsage();
		}

		TEST_CASE("check for memory leaks in nvs_set_blob")
		{
			auto freeHeapBeforeInit = system_get_free_heap_size();
			debug_i("free heap before init = %u", freeHeapBeforeInit);

			REQUIRE(nvsInit(true));

			size_t freeHeapBeforeOpen{0};

			for(int i = 0; i < 500; ++i) {
				uint8_t key[20]{};

				auto myHandle = nvsOpenHandle("leak_check_ns", NVS_READWRITE);
				TEST_ASSERT(myHandle);
				TEST_ASSERT(myHandle->setBlob("key", key, sizeof(key)));
				TEST_ASSERT(myHandle->commit());
				myHandle.reset();

				auto freeHeap = system_get_free_heap_size();
				if(i == 0) {
					freeHeapBeforeOpen = freeHeap;
					debug_i("free heap before open = %u", freeHeapBeforeOpen);
				} else if(freeHeap != freeHeapBeforeOpen) {
					debug_e("free heap after close = %u", freeHeap);
					TEST_ASSERT(false);
				}
			}

			REQUIRE(nvsDeinit());
			auto freeHeap = system_get_free_heap_size();
			if(freeHeap != freeHeapBeforeInit) {
				debug_e("free heap after de-init = %u", freeHeap);
				TEST_ASSERT(false);
			}

			printHeapUsage();
		}

		TEST_CASE("Lots of items")
		{
			REQUIRE(nvsInit(true));
			printHeapUsage();

			nvs::HandlePtr handle;
			REQUIRE_HANDLE(handle = nvsOpenHandle("large space", NVS_READWRITE));
			printHeapUsage();

			size_t entryCount = 0;
			for(unsigned i = 0; i < 10000; ++i) {
				String key = F("key_");
				key += i;
				if(!handle->setString(key, key)) {
					debug_e("setString() failed @%u, err = %u", i, nvs_errno);
					entryCount = i;
					break;
				}

				if(i % 100 == 0) {
					printHeapUsage();
				}
			}

			printHeapUsage();

			for(unsigned i = 0; i < entryCount; ++i) {
				String key = F("key_");
				key += i;
				auto value = handle->getString(key);
				if(value != key) {
					debug_e("getString('%s') failed, err = %u", key.c_str(), nvs_errno);
					break;
				}
			}

			nvs_stats_t stat;
			handle->container().fillStats(stat);
			debug_i("Used entries  = %u", stat.usedEntries);
			debug_i("Free entries  = %u", stat.freeEntries);
			debug_i("Total entries = %u", stat.totalEntries);
			debug_i("Namespaces    = %u", stat.namespaceCount);

			handle.reset();
			printHeapUsage();
		}

		TEST_CASE("Container iterator")
		{
			REQUIRE(listContainer(partNvsTest));

#ifdef ARCH_ESP32
			listContainer("nvs");
#endif
		}
	}

	bool listContainer(const String& name)
	{
		auto container = nvs::partitionManager.lookupContainer(name);
		if(container == nullptr) {
			return false;
		}

		unsigned count{0};
		for(auto item : *container) {
			debug_i("{ namespace: \"%s\", key: \"%s\", dataType: %s, dataSize: %u }", item.nsName().c_str(),
					item.key().c_str(), toString(item.type()).c_str(), item.dataSize());
			++count;
		}
		debug_i("%u items found", count);

		return true;
	}
};

void REGISTER_TEST(Storage)
{
	registerGroup<PartitionTest>();
}
