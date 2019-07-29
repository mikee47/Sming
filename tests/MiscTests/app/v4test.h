/*
 * v4test.h
 *
 * @author: 14 Sep 2018 - mikee47 <mike@sillyhouse.net>
 */

#ifndef __V4TEST_H_
#define __V4TEST_H_

#include "Print.h"

void testBase64();
void testString();
void testMakeHexString();
void testHttpHeaders();
void testHttpCommon();
void testUrl(Print& out);
void testDateTime();
void testObjectMap();
void testSerialBuffer();

void testAll(Print& out);

#endif /* __V4TEST_H_ */
