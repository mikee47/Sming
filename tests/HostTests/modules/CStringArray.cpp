#include <HostTests.h>

#include <Data/CStringArray.h>

class CStringArrayTest : public TestGroup
{
public:
	CStringArrayTest() : TestGroup(_F("CStringArray"))
	{
	}

	void execute() override
	{
		// Use to help diagnose any issues
		/*
		auto debug = [](const CStringArray& csa) {
			debug_hex(DBG, "csa", csa.c_str(), csa.length());
			debug_d("csa.count() = %u", csa.count());
		};
		 */

		// FlashStrings are super-handy for this sort of thing as they're counted
		DEFINE_FSTR_LOCAL(FS_Basic, "a\0"
									"b\0"
									"c\0"
									"d\0");
		DEFINE_FSTR_LOCAL(FS_BasicJoined, "a,b,c,d")

		TEST_CASE("Empty construction")
		{
			// Default object is empty
			CStringArray csa1;
			REQUIRE(csa1.count() == 0);
			CStringArray csa2("");
			REQUIRE(csa2.count() == 0);
			// Contains one empty string
			CStringArray csa3("\0", 1);
			REQUIRE(csa3.count() == 1);
			// Construct using assignment from String
			String s(FS_Basic);
			CStringArray csa4 = s;
			REQUIRE(csa4.count() == 4);
		}

		TEST_CASE("Destruction")
		{
			CStringArray csa = FS_Basic;
			REQUIRE(csa.count() == 4);
			csa.clear();
			REQUIRE(csa.count() == 0);
			// Assignment to null should clear the array
			csa = FS_Basic;
			REQUIRE(csa.count() == 4);
			csa = nullptr;
			REQUIRE(csa.count() == 0);
		}

		TEST_CASE("Single element")
		{
			CStringArray csa = "a";
			REQUIRE(csa.count() == 1);
			csa = "a\0b\0c\0";
			REQUIRE(csa.count() == 1);
		}

		TEST_CASE("Array, no final NUL")
		{
			CStringArray csa = F("a\0"
								 "b\0"
								 "c\0"
								 "d");
			REQUIRE(csa.count() == 4);
			REQUIRE(csa.indexOf("b") == 1);
			REQUIRE(csa.indexOf("d") == 3);
			REQUIRE(csa.indexOf("") == -1);
			REQUIRE(strcmp(csa[0], "a") == 0);
			REQUIRE(csa[4] == nullptr);
		}

		TEST_CASE("Array with final NUL")
		{
			CStringArray csa = FS_Basic;
			REQUIRE(csa.count() == 4);
			REQUIRE(csa.indexOf("d") == 3);
			REQUIRE(csa.indexOf("") == -1);
			REQUIRE(csa[4] == nullptr);

			// Check case-sensitity
			REQUIRE(csa.indexOf("D") == 3);
			REQUIRE(csa.indexOf("D", false) == -1);
		}

		TEST_CASE("Empty value at end")
		{
			CStringArray csa = F("a\0"
								 "b\0"
								 "c\0"
								 "d\0"
								 "\0");
			REQUIRE(csa.count() == 5);
			REQUIRE(csa.indexOf("d") == 3);
			REQUIRE(csa.indexOf("") == 4);
			REQUIRE(strcmp(csa[4], "") == 0);
			REQUIRE(csa[5] == nullptr);
		}

		TEST_CASE("Array concatenation")
		{
			CStringArray csa = F("a\0b\0");
			csa += F("c\0d\0");
			REQUIRE(csa.count() == 4);
		}

		TEST_CASE("Concatenation")
		{
			DEFINE_FSTR_LOCAL(FS_Test1, "1\0"
										"1.50\0"
										"x\0"
										"a\0"
										"b\0"
										"c\0"
										"biscuit\0"
										"1311768467750121216\0");
			CStringArray csa;
			csa += 1;
			csa += 1.5;
			REQUIRE(csa.count() == 2);
			csa += 'x';
			csa += F("a\0b\0c");
			REQUIRE(csa.count() == 6);
			csa += "biscuit";
			csa += 0x12345678ABCDEF00ULL;
			REQUIRE(csa.count() == 8);
			REQUIRE(csa == FS_Test1);

			csa.toUpperCase();
			REQUIRE(csa == F("1\0"
							 "1.50\0"
							 "X\0"
							 "A\0"
							 "B\0"
							 "C\0"
							 "BISCUIT\0"
							 "1311768467750121216\0"));

			REQUIRE(csa.equals(FS_Test1) == false);
			REQUIRE(csa.equalsIgnoreCase(FS_Test1) == true);

			TEST_CASE("Iterators")
			{
				CStringArray::Iterator it1;
				REQUIRE(!it1);

				for(auto it = csa.begin(); it != csa.end(); it++) {
					debug_i("csa[%u] = '%s' @ %u", it.index(), *it, it.offset());
					if(it == "B") {
						it1 = it;
					}
				}

				REQUIRE(it1);
				REQUIRE(it1 == "B");
				REQUIRE(it1 != "b");
				REQUIRE(it1.equalsIgnoreCase("b"));
				REQUIRE(it1.index() == 4);

				it1 = csa.begin();
				REQUIRE(it1);

				auto it2 = csa.end();
				REQUIRE(!it2);
				REQUIRE(it1 != it2);

				it2 = it1;
				REQUIRE(it1 == it2);

				it2++;
				REQUIRE(it1 != it2);

				// Just make sure this compiles OK
				std::swap(it1, it2);
			}
		}

		TEST_CASE("push front")
		{
			CStringArray csa;
			REQUIRE(csa.front() == nullptr);
			REQUIRE(csa.pushFront("abc"));
			REQUIRE_EQ(String(csa.front()), "abc");
			REQUIRE(csa.pushFront("def"));
			REQUIRE_EQ(String(csa.front()), "def");
			REQUIRE_EQ(csa.count(), 2);
			REQUIRE_EQ(csa.popFront(), "def");
			REQUIRE_EQ(csa.count(), 1);
			REQUIRE_EQ(csa.popFront(), "abc");
			REQUIRE_EQ(csa.count(), 0);
			REQUIRE(!csa.popFront());
		}

		TEST_CASE("push back")
		{
			CStringArray csa;
			REQUIRE(csa.back() == nullptr);
			REQUIRE(csa.pushBack("abc"));
			REQUIRE(csa.pushBack("def"));
			REQUIRE_EQ(csa.count(), 2);
			REQUIRE_EQ(String(csa.back()), "def");
			REQUIRE_EQ(csa.popBack(), "def");
			REQUIRE_EQ(String(csa.back()), "abc");
			REQUIRE_EQ(csa.count(), 1);
			REQUIRE_EQ(csa.popBack(), "abc");
			REQUIRE_EQ(csa.count(), 0);
			REQUIRE(csa.back() == nullptr);
			REQUIRE(!csa.popBack());
		}

		TEST_CASE("join")
		{
			CStringArray csa;
			REQUIRE(!csa.join());

			csa = "";
			REQUIRE(csa.join() == "");

			csa.add("a");
			REQUIRE(csa.join() == "a");

			csa.add("");
			REQUIRE(csa.join() == "a,");

			csa.add(F("test\0again"));
			REQUIRE_EQ(csa.join("}+{"), "a}+{}+{test}+{again");
			REQUIRE_EQ(csa.join(nullptr), "atestagain");

			csa = FS_Basic;
			REQUIRE_EQ(csa.join(), FS_BasicJoined);
		}

		TEST_CASE("release")
		{
			String orig;
			// Allocate > SSO
			while(orig.length() <= String::SSO_CAPACITY) {
				orig += FS_Basic;
			}
			CStringArray csa = orig;
			Serial << csa.join() << endl;
			auto cstrWant = csa.c_str();
			String s = csa.release();
			REQUIRE(!csa);
			REQUIRE(s.c_str() == cstrWant);

			REQUIRE(s == orig);

			csa = std::move(s);
			REQUIRE(csa == orig);
		}
	}
};

void REGISTER_TEST(CStringArray)
{
	registerGroup<CStringArrayTest>();
}
