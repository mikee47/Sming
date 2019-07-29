/*
 * test_httpCommon.cpp
 *
 * @author: 4 Oct 2018 - mikee47 <mike@sillyhouse.net>
 */

#include "Network/Http/HttpCommon.h"

void testHttpCommon()
{
	for(int i = 0; i < 100; ++i) {
		auto err = static_cast<http_errno>(i);
		debug_i("httpError(%d) = \"%s\", \"%s\"", i, httpGetErrorName(err).c_str(),
				httpGetErrorDescription(err).c_str());
	}

	for(int i = 100; i < 550; ++i) {
		debug_i("http_status(%d) = \"%s\"", i, httpGetStatusText(i).c_str());
	}
}
