#include "i2c_bus.h"
#include <Clock.h>

void I2CBus::scan(Print& out)
{
	uint8_t err;
	uint8_t addr;
	int nDevices = 0;
	for(addr = 1; addr < 127; addr++) {
		_port->beginTransmission(addr);
		err = _port->endTransmission();
		if(err == 0) {
			out.printf(_F("I2C device found at address 0x%02x !\r\n"), addr);
			nDevices++;
		} else if(err == 4) {
			out.printf(_F("Unknown error at address 0x%02x\r\n"), addr);
		}
	}
	if(nDevices == 0) {
		out.println("No I2C devices found\n");
	} else {
		out.println("done\n");
	}
}

uint16_t I2CBus::readBytes(uint8_t addr, uint8_t* data, uint16_t len, uint16_t delay_ms)
{
	uint16_t ret = 0;
	uint8_t cnt = _port->requestFrom(addr, (uint8_t)len, (uint8_t)1);
	if(cnt == 0) {
		ret = 1 << 13;
	}
	uint16_t index = 0;
	while(_port->available()) {
		if(index > len) {
			return 1 << 14;
		}
		if(delay_ms) {
			delay(delay_ms);
		}
		data[index++] = _port->read();
	}
	return ret;
}

uint16_t I2CBus::readBytes(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t len)
{
	uint16_t ret = 0;
	_port->beginTransmission(addr);
	_port->write(reg);
	_port->endTransmission(false);
	uint8_t cnt = _port->requestFrom(addr, (uint8_t)len, (uint8_t)1);
	if(!cnt) {
		ret = 1 << 13;
	}
	uint16_t index = 0;
	while(_port->available()) {
		if(index > len)
			return 1 << 14;
		data[index++] = _port->read();
	}
	return ret;
}

uint16_t I2CBus::writeBytes(uint8_t addr, uint8_t reg, uint8_t* data, uint16_t len)
{
	uint16_t ret = 0;
	_port->beginTransmission(addr);
	_port->write(reg);
	for(uint16_t i = 0; i < len; i++) {
		_port->write(data[i]);
	}
	ret = _port->endTransmission();
	return ret ? 1 << 12 : ret;
}

bool I2CBus::deviceProbe(uint8_t addr)
{
	uint16_t ret = 0;
	_port->beginTransmission(addr);
	ret = _port->endTransmission();
	return (ret == 0);
}

int I2CBus::readBytes_u16(int addr, uint16_t reg, uint8_t* data, int len)
{
	_port->beginTransmission(addr);
	_port->write(reg >> 8);
	_port->write(reg & 0xFF);
	_port->endTransmission(false);
	uint8_t cnt = _port->requestFrom(addr, len, 1);
	if(!cnt) {
		return 0;
	}
	int index = 0;
	while(_port->available()) {
		if(index > len)
			return 1 << 14;
		data[index++] = _port->read();
	}
	return index == len;
}

int I2CBus::writeBytes_u16(int addr, uint16_t reg, uint8_t* data, int len)
{
	_port->beginTransmission(addr);
	_port->write(reg >> 8);
	_port->write(reg & 0xFF);
	for(int i = 0; i < len; i++) {
		_port->write(data[i]);
	}
	int ret = _port->endTransmission();
	return ret == 0;
}
