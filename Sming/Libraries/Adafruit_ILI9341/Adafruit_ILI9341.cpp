/***************************************************
  This is our library for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
/********************************
 * ported for Sming by M.Bozkurt.
 * alonewolf07@gmail.com
 ********************************/

#include "Adafruit_ILI9341.h"
#include <FlashString/Array.hpp>

#define TFT_DC_DATA digitalWrite(TFT_DC_PIN, true)
#define TFT_DC_COMMAND digitalWrite(TFT_DC_PIN, false)
#define TFT_DC_INIT                                                                                                    \
	pinMode(TFT_DC_PIN, OUTPUT);                                                                                       \
	TFT_DC_DATA

// Reset line is optional - must be tied high if not used
#ifdef TFT_RESET_PIN
#define TFT_RST_ACTIVE digitalWrite(TFT_RESET_PIN, false)
#define TFT_RST_DEACTIVE digitalWrite(TFT_RESET_PIN, true)
#define TFT_RST_INIT                                                                                                   \
	pinMode(TFT_RESET_PIN, OUTPUT);                                                                                    \
	TFT_RST_DEACTIVE
#else
#define TFT_RST_ACTIVE
#define TFT_RST_DEACTIVE
#define TFT_RST_INIT
#endif

// #define MEMORY_ACCESS_CONTROL ILI9341_MADCTL, 1, 0x48				   // ili9341
#define MEMORY_ACCESS_CONTROL 0x40, 1, 0x48, 0x08, 1, 0x48 // ili9341-9340-9340c

// Command, length data
DEFINE_FSTR_ARRAY_LOCAL(							 //
	displayInitData, uint8_t,						 //
	ILI9341_PWCTRA, 5, 0x39, 0x2C, 0x00, 0x34, 0x02, //
	ILI9341_PWCTRB, 3, 0x00, 0XC1, 0X30,			 //
	ILI9341_DRVTMA, 3, 0x85, 0x00, 0x78,			 //
	ILI9341_DRVTMB, 2, 0x00, 0x00,					 //
	ILI9341_PWRSEQ, 4, 0x64, 0x03, 0X12, 0X81,		 //
	ILI9341_PMPRC, 1, 0x20,							 //
	ILI9341_PWCTR1, 1, 0x23,						 // Power control: VRH[5:0]
	ILI9341_PWCTR2, 1, 0x10,						 // Power control: SAP[2:0], BT[3:0]
	ILI9341_VMCTR1, 2, 0x3e, 0x28,					 // VCM control: Contrast
	ILI9341_VMCTR2, 1, 0x86,						 // VCM control2
	MEMORY_ACCESS_CONTROL,							 //
	ILI9341_PIXFMT, 1, 0x55,						 //
	ILI9341_FRMCTR1, 2, 0x00, 0x18,					 //
	ILI9341_DFUNCTR, 3, 0x08, 0x82, 0x27,			 // Display Function Control
	ILI9341_ENA3G, 1, 0x00,							 // 3Gamma Function Disable
	ILI9341_GAMMASET, 1, 0x01,						 // Gamma curve selected
	ILI9341_GMCTRP1, 15, 0x0F, 0x31, 0x2B, 0x0C, 0x0E, 0x08, 0x4E, 0xF1, 0x37, 0x07, 0x10, 0x03, 0x0E, 0x09,
	0x00, // Set Gamma
	ILI9341_GMCTRN1, 15, 0x00, 0x0E, 0x14, 0x03, 0x11, 0x07, 0x31, 0xC1, 0x48, 0x08, 0x0F, 0x0C, 0x31, 0x36,
	0x0F, // Set Gamma
)

//Set communication using HW SPI Port
bool Adafruit_ILI9341::begin(HSPI::PinSet pinSet, uint8_t chipSelect)
{
	if(!Device::begin(pinSet, chipSelect)) {
		return false;
	}
	setSpeed(4000000U);
	setBitOrder(MSBFIRST);
	setClockMode(HSPI::ClockMode::mode0);
	setIoMode(HSPI::IoMode::SPI);

	TFT_DC_INIT;
	TFT_RST_INIT;

	TFT_RST_ACTIVE;
	delayMicroseconds(10000);
	TFT_RST_DEACTIVE;
	delayMicroseconds(1000);

	uint8_t buffer[displayInitData.size()];
	displayInitData.read(0, buffer, displayInitData.size());
	for(unsigned off = 0; off < displayInitData.length();) {
		uint8_t cmd = buffer[off++];
		uint8_t len = buffer[off++];
		debug_hex(DBG, "CMD", buffer + off - 2, len + 2);
		transmitCmdData(cmd, &buffer[off], len);
		off += len;
	}

	transmitCmd(0x11); //Exit Sleep
	delayMicroseconds(120000);

	transmitCmd(0x29); //Display on
	transmitCmd(0x2c);

	return true;
}

void Adafruit_ILI9341::transmitCmdData(uint8_t cmd, const void* data, size_t size)
{
	HSPI::Request req;
	TFT_DC_COMMAND;
	req.out.set8(cmd);
	execute(req);

	TFT_DC_DATA;
	req.out.set(data, size);
	execute(req);
}

void Adafruit_ILI9341::transmitData(uint16_t data)
{
	HSPI::Request req;
	req.out.set16(data);
	execute(req);
}

void Adafruit_ILI9341::transmitCmdData(uint8_t cmd, uint32_t data)
{
	debug_d("[ILI] CMD-DATA 0x%02x 0x%08x", cmd, data);

	HSPI::Request req;

	TFT_DC_COMMAND;
	req.out.set8(cmd);
	execute(req);

	TFT_DC_DATA;
	req.out.set32(data);
	execute(req);
}

void Adafruit_ILI9341::transmitData(uint16_t data, unsigned repeats)
{
	debug_d("[ILI] DATA 0x%04x x %u", data, repeats);

	if(repeats == 0) {
		return;
	}

	constexpr size_t bufLen{128};
	uint16_t buffer[bufLen];
	std::fill_n(buffer, std::min(repeats, bufLen), data);

	HSPI::Request req;
	do {
		auto len = std::min(repeats, bufLen);
		req.out.set(buffer, len * 2);
		repeats -= len;
		execute(req);
	} while(repeats != 0);
}

void Adafruit_ILI9341::transmitCmd(uint8_t cmd)
{
	debug_d("[ILI] CMD 0x%02x", cmd);
	HSPI::Request req;
	TFT_DC_COMMAND;
	req.out.set8(cmd);
	execute(req);
	TFT_DC_DATA;
}

void Adafruit_ILI9341::setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1)
{
	transmitCmdData(ILI9341_CASET, MAKEWORD(x0, x1));
	transmitCmdData(ILI9341_PASET, MAKEWORD(y0, y1));
	transmitCmd(ILI9341_RAMWR); // write to RAM
}

void Adafruit_ILI9341::drawPixel(int16_t x, int16_t y, uint16_t color)
{
	if((x < 0) || (x >= _width) || (y < 0) || (y >= _height))
		return;
	setAddrWindow(x, y, x + 1, y + 1);
	transmitData(SWAPBYTES(color));
}

void Adafruit_ILI9341::pushColor(uint16_t color)
{
	transmitData(SWAPBYTES(color));
}

void Adafruit_ILI9341::pushColors(uint16_t* color, size_t numColors)
{
	for(unsigned i = 0; i < numColors; ++i) {
		color[i] = SWAPBYTES(color[i]);
	}
	HSPI::Request req;
	req.out.set(color, numColors * 2);
	execute(req);
}

void Adafruit_ILI9341::drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color)
{
	// Rudimentary clipping
	if((x >= _width) || (y >= _height))
		return;

	if((y + h - 1) >= _height)
		h = _height - y;

	setAddrWindow(x, y, x, y + h - 1);
	transmitData(SWAPBYTES(color), h);
}

void Adafruit_ILI9341::drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color)
{
	// Rudimentary clipping
	if((x >= _width) || (y >= _height))
		return;
	if((x + w - 1) >= _width)
		w = _width - x;
	setAddrWindow(x, y, x + w - 1, y);
	transmitData(SWAPBYTES(color), w);
}

void Adafruit_ILI9341::fillScreen(uint16_t color)
{
	fillRect(0, 0, _width, _height, color);
}

// fill a rectangle
void Adafruit_ILI9341::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color)
{
	// rudimentary clipping (drawChar w/big text requires this)
	if((x >= _width) || (y >= _height))
		return;
	if((x + w - 1) >= _width)
		w = _width - x;
	if((y + h - 1) >= _height)
		h = _height - y;

	setAddrWindow(x, y, x + w - 1, y + h - 1);
	transmitData(SWAPBYTES(color), h * w);
}

void Adafruit_ILI9341::setRotation(uint8_t m)
{
	uint8_t data;
	rotation = m % 4; // can't be higher than 3
	switch(rotation) {
	case 0:
		data = MADCTL_MX | MADCTL_BGR;
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case 1:
		data = MADCTL_MV | MADCTL_BGR;
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	case 2:
		data = MADCTL_MY | MADCTL_BGR;
		_width = ILI9341_TFTWIDTH;
		_height = ILI9341_TFTHEIGHT;
		break;
	case 3:
		data = MADCTL_MX | MADCTL_MY | MADCTL_MV | MADCTL_BGR;
		_width = ILI9341_TFTHEIGHT;
		_height = ILI9341_TFTWIDTH;
		break;
	}
	transmitCmdData(ILI9341_MADCTL, &data, 1);
}

void Adafruit_ILI9341::invertDisplay(bool i)
{
	transmitCmd(i ? ILI9341_INVON : ILI9341_INVOFF);
}

uint32_t Adafruit_ILI9341::readRegister(uint8_t cmd, uint8_t byteCount)
{
	HSPI::Request req;
	TFT_DC_COMMAND;
	req.setCommand8(cmd);
	req.dummyLen = (byteCount > 1);
	req.in.set32(0, byteCount);
	execute(req);
	TFT_DC_DATA;
	return req.in.data32;
}

void Adafruit_ILI9341::writeMem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const void* data, size_t size)
{
	transmitCmdData(ILI9341_CASET, MAKEWORD(x0, x1));
	transmitCmdData(ILI9341_PASET, MAKEWORD(y0, y1));
	transmitCmd(ILI9341_RAMWR);

	HSPI::Request req;
	req.out.set(data, size);
	execute(req);
}

void Adafruit_ILI9341::readMem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void* buffer, size_t size)
{
	transmitCmdData(ILI9341_CASET, MAKEWORD(x0, x1));
	transmitCmdData(ILI9341_PASET, MAKEWORD(y0, y1));

	HSPI::Request req;
	TFT_DC_COMMAND;
	req.setCommand8(ILI9341_RAMRD);
	req.dummyLen = 8;
	req.in.set(buffer, size);
	execute(req);
	TFT_DC_DATA;
}
