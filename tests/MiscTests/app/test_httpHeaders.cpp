/*
 * test_httpHeaders.cpp
 *
 * @author: 4 Oct 2018 - mikee47 <mike@sillyhouse.net>
 */

#include "Network/Http/HttpHeaders.h"
#include "Network/WebConstants.h"
#include "Services/Profiling/ElapseTimer.h"

static void printHeaders(const HttpHeaders& headers)
{
	debugf("  count: %d", headers.count());
	for(unsigned i = 0; i < headers.count(); ++i) {
		String s = headers[i];
		m_printHex("  ", s.c_str(), s.length(), 0, 32);
	}
}

void profileHttpHeaders()
{
	debugf("\nPROFILING");

	// Allocate everything on the heap so we can track memory usage
	auto freeHeap = system_get_free_heap_size();

	auto headersPtr = new HttpHeaders;
	HttpHeaders& headers = *headersPtr;

	// Set header values
	ElapseTimer timer;
	headers[HTTP_HEADER_CONTENT_ENCODING] = _F("gzip");
	headers[HTTP_HEADER_CONTENT_LENGTH] = 6042;
	headers[HTTP_HEADER_ETAG] = "00f-3d-179a0-0";
	headers[HTTP_HEADER_CONNECTION] = _F("keep-alive");
	headers[HTTP_HEADER_SERVER] = _F("HttpServer/Sming");
	headers[HTTP_HEADER_CONTENT_TYPE] = ContentType::toString(MIME_JS);
	headers[HTTP_HEADER_CACHE_CONTROL] = "max-age=31536000, public";
	headers[HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN] = "*";
	auto standardElapsed = timer.elapsed();
	headers[F("X-Served-By")] = _F("Donkey Kong");
	headers[F("Vary")] = _F("Accept-Encoding");
	headers[F("X-Fastly-Request-ID")] = _F("38ef411e0ec3bf681d29d8b4b51f3516d3ef9e03");
	auto totalElapsed = timer.elapsed();
	debugf("Set header values");
	debugf("  Elapsed standard: %u, total: %u, heap used: %u", standardElapsed, totalElapsed,
		   freeHeap - system_get_free_heap_size());

	// Query header value by field name
	auto queryByEnum = [&](HttpHeaderFieldName name) {
		debugf("  header[\"%s\"]: %s", headers.toString(name).c_str(), headers[name].c_str());
	};
	auto queryByString = [&](const String& name) {
		debugf("  header[\"%s\"]: %s", name.c_str(), headers[name].c_str());
	};
	debugf("Query header values");
	timer.start();
	queryByEnum(HTTP_HEADER_CONTENT_ENCODING);
	queryByEnum(HTTP_HEADER_CONTENT_LENGTH);
	queryByEnum(HTTP_HEADER_ETAG);
	queryByEnum(HTTP_HEADER_CONNECTION);
	queryByEnum(HTTP_HEADER_SERVER);
	queryByEnum(HTTP_HEADER_CONTENT_TYPE);
	queryByEnum(HTTP_HEADER_CACHE_CONTROL);
	queryByEnum(HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN);
	standardElapsed = timer.elapsed();
	queryByString("X-Served-By");
	queryByString("Vary");
	queryByString("X-Fastly-Request-ID");
	totalElapsed = timer.elapsed();
	debugf("  Elapsed standard: %u, total: %u", standardElapsed, totalElapsed);

	// Print header values - accessed by index
	debugf("Printing %u headers", headers.count());
	timer.start();
	printHeaders(headers);
	debugf("  Elapsed: %u", timer.elapsed());

	delete headersPtr;
}

void testHttpHeaders()
{
	HttpHeaders headers;

	headers["X-Auth-Header"] = "Value Data";
	headers["x-Auth-header"] = "Value Data New";

	debugf("Header: %s", headers["X-AUTH-HEADER"].c_str());
	printHeaders(headers);

	headers.remove("x-auth-header");
	printHeaders(headers);

	headers[HTTP_HEADER_ACCESS_CONTROL_ALLOW_ORIGIN] = "This is access control origin";

	debugf("Header: %s", headers["access-control-Allow-Origin"].c_str());

	debugf("Headers count: %d", headers.count());

	headers.clear();
	printHeaders(headers);

	const HttpHeaders& constHeaders = headers;
	debugf("Non-Existent Header (const): \"%s\"", constHeaders["Non-Existent"].c_str());
	printHeaders(headers);

	debugf("Non-Existent Header (non-const): \"%s\"", headers["Non-Existent"].c_str());
	printHeaders(headers);

	headers["X-Auth-Header"] = "Value Data";
	debugf("set(\"X-Auth-Header\")");
	printHeaders(headers);

	headers["x-Auth-header"] = "Value Data New";
	debugf("set(\"x-Auth-Header\")");
	printHeaders(headers);

	debugf("Header[\"X-AUTH-HEADER\"]: \"%s\"", headers["X-AUTH-HEADER"].c_str());
	printHeaders(headers);

	headers.remove("x-auth-header");
	debugf("headers.remove(\"x-auth-header\")");
	printHeaders(headers);

	headers["Mary"] = "Had a little lamb";
	headers[HTTP_HEADER_CONTENT_LENGTH] = "12345";
	headers["George"] = "Was an idiot";
	printHeaders(headers);

	debugf("testing setMultiple()");
	HttpHeaders headers2;
	headers2.setMultiple(headers);
	printHeaders(headers2);

	profileHttpHeaders();
}
