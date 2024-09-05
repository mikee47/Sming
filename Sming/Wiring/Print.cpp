/* $Id: Print.cpp 1156 2011-06-07 04:01:16Z bhagman $
||
|| @author         Hernando Barragan <b@wiring.org.co>
|| @url            http://wiring.org.co/
|| @contribution   Nicholas Zambetti
|| @contribution   Brett Hagman <bhagman@wiring.org.co>
|| @contribution   Alexander Brevig <abrevig@wiring.org.co>
||
|| @description
|| | Print library.
|| |
|| | Wiring Common API
|| #
||
|| @license Please see cores/Common/License.txt.
||
*/

#include "Print.h"
#include "WString.h"
#include <stringconversion.h>
#include <cmath>
#include <algorithm>

/*
|| @description
|| | Virtual method - may be redefined in derived class (polymorphic)
|| | write()s a specific length string.
|| #
*/

size_t Print::write(const uint8_t* buffer, size_t size)
{
	size_t n = 0;
	while(size--) {
		n += write(*buffer++);
	}
	return n;
}

size_t Print::printf(const char* fmt, ...)
{
	size_t buffSize = INITIAL_PRINTF_BUFFSIZE;
	for(;;) {
		char tempBuff[buffSize];
		va_list va;
		va_start(va, fmt);
		size_t sz = m_vsnprintf(tempBuff, buffSize, fmt, va);
		va_end(va);
		if(sz > (buffSize - 1)) {
			buffSize = sz + 1; // Leave room for terminating null char
			continue;		   // try again
		}

		if(sz > 0) {
			write(tempBuff, sz);
		}

		return sz;
	}
}

size_t Print::printNumber(unsigned long num, uint8_t base, uint8_t width, char pad)
{
	char buf[8 * sizeof(num) + 1]; // Assumes 8-bit chars plus zero byte.
	ultoa_wp(num, buf, base, width, pad);
	return write(buf);
}

size_t Print::printNumber(const unsigned long long& num, uint8_t base, uint8_t width, char pad)
{
	char buf[8 * sizeof(num) + 1]; // Assumes 8-bit chars plus zero byte.
	ulltoa_wp(num, buf, base, width, pad);
	return write(buf);
}

size_t Print::printNumber(long num, uint8_t base, uint8_t width, char pad)
{
	char buf[8 * sizeof(num) + 1]; // Assumes 8-bit chars plus zero byte.
	ltoa_wp(num, buf, base, width, pad);
	return write(buf);
}

size_t Print::printNumber(const long long& num, uint8_t base, uint8_t width, char pad)
{
	char buf[8 * sizeof(num) + 1]; // Assumes 8-bit chars plus zero byte.
	lltoa_wp(num, buf, base, width, pad);
	return write(buf);
}

size_t Print::printFloat(double number, uint8_t digits)
{
	char buf[64];
	dtostrf(number, 0, digits, buf);
	return print(buf);
}
