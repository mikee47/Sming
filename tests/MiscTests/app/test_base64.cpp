/*
 * base64.cpp
 *
 *  Created on: 14 Sep 2018
 *      Author: mikee47
 */

#include "debug_progmem.h"

#include "Services/WebHelpers/base64.h"

void testBase64()
{
	m_puts(_F("testBase64\n"));

	String user("donkey");
	String pass("kingpin");
	String token = '\0' + user + '\0' + pass;
	m_printHex(_F("encode input"), token.c_str(), token.length() + 1);
	String hash = base64_encode(token);
	m_printHex(_F("encode output"), hash.c_str(), hash.length() + 1);
	String clear = base64_decode(hash);
	m_printHex(_F("decode output"), clear.c_str(), clear.length() + 1);
}
