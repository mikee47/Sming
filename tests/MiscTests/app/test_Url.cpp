/*
 * test_URL.cpp
 *
 * @author: 8 Oct 2018 - mikee47 <mike@sillyhouse.net>
 */

#include "Network/Url.h"
#include "Network/Http/HttpRequest.h"
#include "Network/Http/HttpBodyParser.h"
#include "Data/Stream/UrlencodedOutputStream.h"

const char* testUrls[] = {
	"http://lightcon.local?param1=1&param2=2#whacky%20races",
	"http://lightcon.local/iocontrol.js?cid=fa373784",
	"/iocontrol.js?cid=81e66a3a",
};

void printParams(const HttpParams& params)
{
	for(unsigned j = 0; j < params.count(); ++j) {
		debugf("  Param #%u: %s = \"%s\"", j, params.keyAt(j).c_str(), params.valueAt(j).c_str());
	}
}

void testUrl(Print& out)
{
	for(unsigned i = 0; i < ARRAY_SIZE(testUrls); ++i) {
		const char* urlText = testUrls[i];
		debugf("URL #%-3u \"%s\"", i, urlText);
		Url(urlText).debugPrintTo(out);
	}

	debugf("formUrlParser test");
	for(unsigned i = 0; i < ARRAY_SIZE(testUrls); ++i) {
		const char* urlText = testUrls[i];
		debugf("URL #%-3u \"%s\"", i, urlText);
		Url url(urlText);
		String query = url.Query;
		const char* p = query.c_str();
		++p; // Skip the '?'
		HttpRequest request;
		formUrlParser(request, nullptr, PARSE_DATASTART);
		while(*p) {
			formUrlParser(request, p++, 1);
		}
		formUrlParser(request, nullptr, PARSE_DATAEND);
		printParams(request.postParams);
		debugf("cid = %s", request.getPostParameter("cid").c_str());
	}

	debugf("HttpRequest getQueryParameter() test");
	HttpRequest request;
	request.uri = testUrls[1];
	debugf("URL = \"%s\"", request.uri.toString().c_str());
	debugf("cid = %s", request.getQueryParameter("cid").c_str());

	debugf("HttpRequest postParams test");
	HttpParams params;
	params["param 1"] = "Mary had a little lamb,";
	params["param 2"] = "It's fleece was very red.";
	params["param 3"] = "The reason for this was, you see";
	params["param 4"] = "It had a pickaxe through its head.";
	UrlencodedOutputStream stream(params);
	char buffer[64];
	while(!stream.isFinished()) {
		unsigned count = stream.readMemoryBlock(buffer, sizeof(buffer));
		m_nputs(buffer, count);
		stream.seek(count);
	}
	m_putc('\n');

	debugf("Putting it all together...");
	auto& uri = request.uri;
	uri.Query.setMultiple(params);
	uri.Fragment = "I told you this would happen";
	uri.debugPrintTo(out);

	debugf("Alternative construction");
	{
#define MQTT_SERVER "192.168.2.138"
#define MQTT_PORT 1883
#define LOG ""
#define PASS ""
		Url url(URI_SCHEME_MQTT, F(LOG), F(PASS), F(MQTT_SERVER), MQTT_PORT);
		out.println(url);
	}
	{
		Url url("http://simple.anakod.ru/templates/jquery.js.gz");
		out.println(url);
	}
	{
		Url uri;
		uri.Host = "api.thingspeak.com";
		uri.Path = "/update";
		uri.Query["key"] = "7XXUJWCWYTMXKN3L";
		int sensorValue = 347;
		uri.Query["field1"] = String(sensorValue);
		out.println(uri);
	}
}
