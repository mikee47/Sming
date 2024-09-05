#include <HostTests.h>
#include <FlashString/Array.hpp>

static int num_instances;

struct A {
	A() : order(++num_instances)
	{
	}

	int order;
};

namespace libc_initorder
{
A a3;
A a4;
A a2 __attribute__((init_priority(1000)));
A a1 __attribute__((init_priority(990)));
} // namespace libc_initorder

// Simple check to determine if a pointer refers to ROM
#define isRomPtr(ptr) (uint32_t(ptr) >= 0x40000000 && uint32_t(ptr) < 0x40100000)

/*
	Format                              Exponent   Mantissa (effective bits)
	Float32 IEEE754 single-precision    8          23 (24)
	Float64 IEEE754 double-precision    11         52 (53)
*/
#define FLOAT32_TEST_MAP(XX)                                                                                           \
	XX(0x0100'0000L, "16777216.00000000", "16777216.0")                                                                \
	XX(-0x0100'0000L, "-16777216.00000000", "-16777216.0")                                                             \
	XX(0x0100'0000L / 1000.0, "16777.21679687", "16777.21679687")                                                      \
	XX(4294967040 * 10000.0, "42949668765696.00000000", "42949668765696.0")                                            \
	XX(-4294967040, "-4294967040.00000000", "-4294967040.0")                                                           \
	XX(std::numeric_limits<float>::infinity(), "Inf", "Inf")                                                           \
	XX(-std::numeric_limits<float>::infinity(), "Inf", "Inf")                                                          \
	XX(3.1415926535897932384626433, "3.14159274", "3.14159274")                                                        \
	XX(32766.6612999999997555, "32766.66210937", "32766.66210937")                                                     \
	XX(1.6613, "1.66129994", "1.66129994")                                                                             \
	XX(-32766.1234, "-32766.12304687", "-32766.12304687")                                                              \
	XX(0.0, "0.00000000", "0.0")                                                                                       \
	XX(-0.0, "0.00000000", "0.0")                                                                                      \
	XX(100.0, "100.00000000", "100.0")

#define FLOAT64_TEST_MAP(XX)                                                                                           \
	XX(0x0020'0000'0000'0000LL, "9007199254740992.0000000000000000", "9007199254740992.0")                             \
	XX(-0x0020'0000'0000'0000LL, "-9007199254740992.0000000000000000", "-9007199254740992.0")                          \
	XX(4294967040 + 1, "4294967041.0000000000000000", "4294967041.0")                                                  \
	XX(-4294967040 - 1, "-4294967041.0000000000000000", "-4294967041.0")                                               \
	XX(3.1415926535897932384626433, "3.1415926535897931", "3.1415926535897931")                                        \
	XX(32766.6613, "32766.6612999999997555", "32766.6612999999997555")                                                 \
	XX(1.6613, "1.6612999999999999", "1.6612999999999999")                                                             \
	XX(-32766.1234, "-32766.1234000000004016", "-32766.1234000000004016")                                              \
	XX(9223372036854775808.0, "9223372036854775808.0000000000000000", "9223372036854775808.0")                         \
	XX(-9223372036854775808.0, "-9223372036854775808.0000000000000000", "-9223372036854775808.0")                      \
	XX(3.4e18, "3400000000000000000.0000000000000000", "3400000000000000000.0")                                        \
	XX((double)std::numeric_limits<int64_t>::max(), "9223372036854775808.0000000000000000", "9223372036854775808.0")   \
	XX((double)std::numeric_limits<int64_t>::min(), "-9223372036854775808.0000000000000000", "-9223372036854775808.0")

template <typename T> struct FloatTest {
	T num;
	PGM_P ref;
	PGM_P str1;
	PGM_P str2;
};

#define XX(num, str1, str2) {num, STR(num), str1, str2},
DEFINE_FSTR_ARRAY_LOCAL(float32_tests, FloatTest<float>, FLOAT32_TEST_MAP(XX))
DEFINE_FSTR_ARRAY_LOCAL(float64_tests, FloatTest<double>, FLOAT64_TEST_MAP(XX))
#undef XX

/*
 * Anything to do with main C/C++ libraries, startup code, etc.
 */
class LibcTest : public TestGroup
{
public:
	LibcTest() : TestGroup(_F("LibC"))
	{
	}

	void execute() override
	{
		// https://github.com/esp8266/Arduino/issues/6870
		TEST_CASE("Global object init order")
		{
			using namespace libc_initorder;
			debug_i("order: %d, %d, %d, %d", a1.order, a2.order, a3.order, a4.order);
			// Compilers may legitimately order these in two different ways
			if(a1.order == 3) {
				REQUIRE(a1.order == 3);
				REQUIRE(a2.order == 4);
				REQUIRE(a3.order == 1);
				REQUIRE(a4.order == 2);
			} else {
				REQUIRE(a1.order == 1);
				REQUIRE(a2.order == 2);
				REQUIRE(a3.order == 3);
				REQUIRE(a4.order == 4);
			}
		}

#ifdef ARCH_ESP8266
		TEST_CASE("functions in ROM")
		{
			REQUIRE(isRomPtr(memcpy));
			REQUIRE(isRomPtr(memmove));
			REQUIRE(isRomPtr(memset));
			REQUIRE(isRomPtr(memcmp));
			REQUIRE(isRomPtr(strlen));
			REQUIRE(isRomPtr(strcmp));
			REQUIRE(isRomPtr(strncmp));
			REQUIRE(isRomPtr(strstr));
		}
#endif

		TEST_CASE("64-bit mprintf")
		{
			char buffer[256];
			m_snprintf(buffer, sizeof(buffer), "%x", 0x12345678);
			REQUIRE_EQ(String(buffer), "12345678");
			m_snprintf(buffer, sizeof(buffer), "%u", 12345678);
			REQUIRE_EQ(String(buffer), "12345678");
			m_snprintf(buffer, sizeof(buffer), "%llx", 0x123456789ABCDEFULL);
			REQUIRE_EQ(String(buffer), "123456789abcdef");
			m_snprintf(buffer, sizeof(buffer), "0x%016llX", 0x123456789ABCDEFULL);
			REQUIRE_EQ(String(buffer), "0x0123456789ABCDEF");
			m_snprintf(buffer, sizeof(buffer), "%llu", 123456789123456789ULL);
			REQUIRE_EQ(String(buffer), "123456789123456789");
		}

		TEST_CASE("Floating-point printing")
		{
			checkFloats(float32_tests, 8);
			checkFloats(float64_tests, 16);
		}
	}

	template <typename T> void checkFloats(const FSTR::Array<FloatTest<T>>& tests, int precision)
	{
		for(auto test : tests) {
			Serial << test.ref << ": ";
			CHECK_EQ(String(test.str1), String(test.num, precision));
			CHECK_EQ(String(test.str2), String(test.num, -precision));
			Serial << "OK" << endl;
		}
	}
};

void REGISTER_TEST(Libc)
{
	registerGroup<LibcTest>();
}
