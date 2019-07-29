/*
 * test_SerialBuffer.cpp
 *
 * @author: 7 Mar 2019 - mikee47 <mike@sillyhouse.net>
 */

#include <user_config.h>
#include "SerialBuffer.h"

void testSerialBuffer()
{
	SerialBuffer txbuf;
	txbuf.resize(128);

	auto dump = [&]() {
		char buf[32];
		unsigned count;
		while((count = txbuf.read(buf, sizeof(buf))) != 0) {
			m_nputs(buf, count);
		}
	};

	m_printf("Free space = %u bytes\n", txbuf.getFreeSpace());
	m_printf("Available = %u bytes\n", txbuf.available());

	char buffer[64];
	memset(buffer, 'A', sizeof(buffer));
	auto count = txbuf.write(buffer, sizeof(buffer));
	m_printf("Wrote %u bytes\n", count);
	m_printf("Available = %u bytes\n", txbuf.available());
	memset(buffer, 0, sizeof(buffer));
	count = txbuf.read(buffer, sizeof(buffer));
	m_printf("Read %u bytes\n", count);
	m_nputs(buffer, count);

	for(unsigned i = 0; i < 10; ++i) {
		m_printf("txfree = %u\n", txbuf.getFreeSpace());
		for(char c = 'a'; c <= 'z'; ++c) {
			if(txbuf.getFreeSpace() < 10) {
				dump();
			}
			//      txbuf.writeChar(c);
			char tmp = c;
			txbuf.write(&tmp, 1);
		}
		txbuf.writeChar('\n');
	}
	dump();
}
