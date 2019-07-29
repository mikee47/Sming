/*
 * v4test.cpp
 *
 * @author: 17 Sep 2018 - mikee47 <mike@sillyhouse.net>
 */

#include "v4test.h"
#include "Network/Http/HttpCommon.h"
#include "Network/Http/HttpRequest.h"
#include "DateTime.h"
#include "IPAddress.h"

static void testStringArg(String arg)
{
	debug_i("arg: %s", arg.c_str());
}

void testHttpStringConstruction()
{
	http_method method = HTTP_GET;
	HttpRequest request(URL("/index.html"));
	String s = http_method_str(method) + ' ' + request.uri.getPathWithQuery() + _F(" HTTP/1.1\r\n");
	debug_i("s = '%s'", s.c_str());

	s = String(http_method_str(method)) + ' ' + request.uri.getPathWithQuery() + _F(" HTTP/1.1\r\n");
	debug_i("s = '%s'", s.c_str());

	s = http_method_str(method) + String(' ') + request.uri.getPathWithQuery() + _F(" HTTP/1.1\r\n");
	debug_i("s = '%s'", s.c_str());

	String tag = "tag";
	s = '"' + tag + '"';
	debug_i("s = '%s'", s.c_str());

	tag = "tag";
	s = String('"' + tag + '"');
	debug_i("s = '%s'", s.c_str());

	tag = "tag";
	s = String('"') + tag + '"';
	debug_i("s = '%s'", s.c_str());

	testStringArg(_F("string"));
	testStringArg(F("string"));
}

void testIPAddress()
{
	auto test = [](const String& str) {
		IPAddress test(str);
		debug_i("test('%s') = '%s'", str.c_str(), test.toString().c_str());
	};

	test("1.2.3.4.5");
	test("1.2.3.4.");
	test("1.2.3.4");
	test("1.2.3.");
	test("1.2.3");
	test("1.2.");
	test("1.2");
	test("1.");
	test("1");
	test("");
	test(nullptr);
}

void testAll(Print& out)
{
	testIPAddress();
	testDateTime();

	testUrl(out);
	//	testHttpCommon();
	testHttpHeaders();
	//	testString();
	//	testMakeHexString();
	//	testBase64();
}
