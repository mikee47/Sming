#include <HostTests.h>

class FilesTest : public TestGroup
{
public:
	FilesTest() : TestGroup(_F("files"))
	{
	}

	void execute() override
	{
		const FlashString& testContent = Resource::README_md;

		debug_i("testContent.length = 0x%08x", testContent.length());

		DEFINE_FSTR_LOCAL(testFileName, "test.txt");

		int res, pos, size;

		{
			FileHandle file{-1};

			TEST_CASE("Initial position and size")
			{
				res = fileSetContent(testFileName, testContent);
				debug_i("fileSetContent() returned %d", res);
				REQUIRE(size_t(res) == testContent.length());
				file = fileOpen(testFileName, File::ReadWrite);
				size = fileSeek(file, 0, SeekOrigin::End);
				pos = fileSeek(file, 100, SeekOrigin::Start);
				debug_i("pos = %d, size = %d", pos, size);
				REQUIRE(pos == 100);
				REQUIRE(size_t(size) == testContent.length());
			}

			TEST_CASE("Reduce file sizes")
			{
				res = fileTruncate(file, 555);
				pos = fileTell(file);
				size = fileSeek(file, 0, SeekOrigin::End);
				debug_i("res = %d, pos = %d, size = %d", res, pos, size);
				REQUIRE(res == 0);
				REQUIRE(pos == 100);
				REQUIRE(size == 555);
			}

			TEST_CASE("Increase file size")
			{
				res = fileTruncate(file, 12345);
				size = fileSeek(file, 0, SeekOrigin::End);
				debug_i("res = %d, size = %d", res, size);
				REQUIRE(res < 0);
				REQUIRE(size == 555);
			}

			TEST_CASE("Close file")
			{
				fileClose(file);
				REQUIRE(fileTell(file) < 0);
			}
		}

		TEST_CASE("fileGetSize")
		{
			size = fileGetSize(testFileName);
			debug_i("size = %u", size);
			REQUIRE(size == 555);
		}

		TEST_CASE("Truncate by file name, increase size")
		{
			fileSetContent(testFileName, testContent);
			res = fileTruncate(testFileName, 34500);
			size = fileGetSize(testFileName);
			debug_i("fileTruncate() returned %d, size = %d", res, size);
			REQUIRE(res < 0);
			REQUIRE(size_t(size) == testContent.length());
		}

		TEST_CASE("Truncate by file name, reduce size")
		{
			res = fileTruncate(testFileName, 345);
			size = fileGetSize(testFileName);
			debug_i("fileTruncate() returned %d, size = %d", res, size);
			REQUIRE(res == 0);
			REQUIRE(size == 345);
		}

		TEST_CASE("Truncate read-only file stream")
		{
			fileSetContent(testFileName, testContent);
			FileStream fs(testFileName);
			Serial << testFileName << " ID: " << fs.id() << endl;
			res = fs.truncate(100);
			pos = fs.getPos();
			size = fs.getSize();
			debug_i("fs.truncate() returned %d, pos = %d, size = %d", res, pos, size);
			REQUIRE(res == int(false));
			REQUIRE(pos == 0);
			REQUIRE(size_t(size) == testContent.length());
			fs.close();
			size = fileGetSize(testFileName);
			debug_i("Actual file size = %d", size);
			REQUIRE(size_t(size) == testContent.length());
		}

		TEST_CASE("Truncate read/write file stream")
		{
			FileStream fs;
			fs.open(testFileName, File::ReadWrite);
			fs.seek(50);
			res = fs.truncate(100);
			pos = fs.getPos();
			size = fs.getSize();
			debug_i("fs.truncate() returned %d, pos = %d, size = %d", res, pos, size);
			REQUIRE(res == int(true));
			REQUIRE(pos == 50);
			REQUIRE(size == 100);
			fs.close();
			size = fileGetSize(testFileName);
			debug_i("Actual file size = %d", size);
			REQUIRE(size == 100);
		}

		TEST_CASE("Seek file stream past end of file")
		{
			FileStream fs;
			fs.open(testFileName, File::ReadWrite);
			res = fs.seekFrom(101, SeekOrigin::Start);
			pos = fs.getPos();
			size = fs.getSize();
			debug_i("fs.seekFrom() returned %d, pos = %d, size = %d", res, pos, size);
			REQUIRE(res == 100);
			REQUIRE(pos == 100);
			REQUIRE(size == 100);
			fs.close();
			size = fileGetSize(testFileName);
			debug_i("Actual file size = %d", size);
			REQUIRE(size == 100);
		}
	}
};

void REGISTER_TEST(Files)
{
	registerGroup<FilesTest>();
}
