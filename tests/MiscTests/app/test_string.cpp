/*
 * test_string.cpp
 *
 *  Created on: 14 Sep 2018
 *      Author: mikee47
 */

#include "debug_progmem.h"
#include "WString.h"
#include "Data/HexString.h"

void testString()
{
	m_puts(_F("testString\n"));

	String text = F("The quick brown fox jumps over a lazy dog.");

	auto replace = [&](const String& from, const String& to) {
		text.replace(from, to);
		m_printHex(_F("from"), from.c_str(), from.length());
		m_printHex(_F("to"), to.c_str(), to.length());
		m_printHex(_F("result"), text.c_str(), text.length() + 1);
	};

	m_printHex(_F("Text"), text.c_str(), text.length() + 1);

	replace(" ", String("\x00!\x00", 3));
	replace(String("\x00", 1), nullptr);
	replace("!", " ");

	replace("a", "\x02");
	replace("b", String("\x00\bad\x00", 5));

	m_printHex(_F("Text"), text.c_str(), text.length() + 1);

	static DEFINE_FSTR(test, "This is a some test data \1\2\3 Not all ASCII\0\0\0Anything, really\0Can I go home now?");
	LOAD_FSTR(data, test);
	m_printHex(_F("TEST1"), data, test.length());
	m_printHex(_F("TEST2"), data, test.length(), 0);
	m_printHex(_F("TEST3"), data, test.length(), 0xFFFFF9, 8);
	m_printHex(nullptr, data, test.length(), 0x7FFFFFF9, 0);

	debugf("\nTest String concat");
	String path = "/path/to";
	String query;

	debugf("path = \"%s\"", path.c_str());
	debugf("query = \"%s\"", query.c_str());

	String tmpString = path + query;
	debugf("path + query = \"%s\"", tmpString.c_str());

	String tmpString2 = query + path;
	debugf("query + path = \"%s\"", tmpString2.c_str());
}

void testMakeHexString()
{
	uint8 hwaddr[6];
	wifi_get_macaddr(STATION_IF, hwaddr);

	m_printHex("MAC", hwaddr, 6);

	auto test = [](const uint8_t* data, unsigned length, char separator = '\0') {
		String s = makeHexString(data, length, separator);
		m_printf("test: data = %p, length = %u, separator = 0x%02x, result = %u chars\r\n", data, length, separator,
				 s.length());
		m_printHex(nullptr, s.c_str(), s.length(), -1, 24);
	};

	test(nullptr, 6);
	test(hwaddr, 0);
	test(hwaddr, 6);
	test(hwaddr, 6, ':');
	test(hwaddr, 7);
	test(hwaddr, 7, ':');
	test(hwaddr, 1, ':');
	test(hwaddr, 0, ':');
}
