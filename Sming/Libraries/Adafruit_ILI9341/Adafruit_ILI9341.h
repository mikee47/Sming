/***************************************************
  This is our library for the Adafruit  ILI9341 Breakout and Shield
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

#ifndef _ADAFRUIT_ILI9341H_
#define _ADAFRUIT_ILI9341H_

#include <Digital.h>
#include <Libraries/Adafruit_GFX/Adafruit_GFX.h>
#include <HSPI/Device.h>

#define ILI9341_TFTWIDTH 240
#define ILI9341_TFTHEIGHT 320

#define ILI9341_NOP 0x00
#define ILI9341_SWRESET 0x01
#define ILI9341_RDDID 0x04
#define ILI9341_RDDST 0x09

#define ILI9341_SLPIN 0x10
#define ILI9341_SLPOUT 0x11
#define ILI9341_PTLON 0x12
#define ILI9341_NORON 0x13

#define ILI9341_RDMODE 0x0A
#define ILI9341_RDMADCTL 0x0B
#define ILI9341_RDPIXFMT 0x0C
#define ILI9341_RDIMGFMT 0x0D
#define ILI9341_RDSIGMODE 0x0E
#define ILI9341_RDSELFDIAG 0x0F

#define ILI9341_INVOFF 0x20
#define ILI9341_INVON 0x21
#define ILI9341_GAMMASET 0x26
#define ILI9341_DISPOFF 0x28
#define ILI9341_DISPON 0x29

#define ILI9341_CASET 0x2A
#define ILI9341_PASET 0x2B
#define ILI9341_RAMWR 0x2C
#define ILI9341_COLSET 0x2D
#define ILI9341_RAMRD 0x2E

#define ILI9341_PTLAR 0x30
#define ILI9341_MADCTL 0x36
#define ILI9341_PIXFMT 0x3A

#define ILI9341_FRMCTR1 0xB1
#define ILI9341_FRMCTR2 0xB2
#define ILI9341_FRMCTR3 0xB3
#define ILI9341_INVCTR 0xB4
#define ILI9341_DFUNCTR 0xB6

#define ILI9341_PWCTR1 0xC0
#define ILI9341_PWCTR2 0xC1
#define ILI9341_PWCTR3 0xC2
#define ILI9341_PWCTR4 0xC3
#define ILI9341_PWCTR5 0xC4
#define ILI9341_VMCTR1 0xC5
#define ILI9341_VMCTR2 0xC7

#define ILI9341_PWCTRA 0xCB
#define ILI9341_PWCTRB 0xCF

#define ILI9341_RDID1 0xDA
#define ILI9341_RDID2 0xDB
#define ILI9341_RDID3 0xDC
#define ILI9341_RDID4 0xDD

#define ILI9341_GMCTRP1 0xE0
#define ILI9341_GMCTRN1 0xE1

#define ILI9341_DRVTMA 0xE8
#define ILI9341_DRVTMB 0xEA
#define ILI9341_PWRSEQ 0xED

#define ILI9341_ENA3G 0xF2
#define ILI9341_PMPRC 0xF7

// MADCTL register bits
#define MADCTL_MY 0x80
#define MADCTL_MX 0x40
#define MADCTL_MV 0x20
#define MADCTL_ML 0x10
#define MADCTL_RGB 0x00
#define MADCTL_BGR 0x08
#define MADCTL_MH 0x04

// Color definitions
#define ILI9341_BLACK 0x0000	   /*   0,   0,   0 */
#define ILI9341_NAVY 0x000F		   /*   0,   0, 128 */
#define ILI9341_DARKGREEN 0x03E0   /*   0, 128,   0 */
#define ILI9341_DARKCYAN 0x03EF	/*   0, 128, 128 */
#define ILI9341_MAROON 0x7800	  /* 128,   0,   0 */
#define ILI9341_PURPLE 0x780F	  /* 128,   0, 128 */
#define ILI9341_OLIVE 0x7BE0	   /* 128, 128,   0 */
#define ILI9341_LIGHTGREY 0xC618   /* 192, 192, 192 */
#define ILI9341_DARKGREY 0x7BEF	/* 128, 128, 128 */
#define ILI9341_BLUE 0x001F		   /*   0,   0, 255 */
#define ILI9341_GREEN 0x07E0	   /*   0, 255,   0 */
#define ILI9341_CYAN 0x07FF		   /*   0, 255, 255 */
#define ILI9341_RED 0xF800		   /* 255,   0,   0 */
#define ILI9341_MAGENTA 0xF81F	 /* 255,   0, 255 */
#define ILI9341_YELLOW 0xFFE0	  /* 255, 255,   0 */
#define ILI9341_WHITE 0xFFFF	   /* 255, 255, 255 */
#define ILI9341_ORANGE 0xFD20	  /* 255, 165,   0 */
#define ILI9341_GREENYELLOW 0xAFE5 /* 173, 255,  47 */
#define ILI9341_PINK 0xF81F

inline uint16_t SWAPBYTES(uint16_t w)
{
	return (w >> 8) | (w << 8);
}

inline uint32_t MAKEWORD(uint8_t b1, uint8_t b2, uint8_t b3, uint8_t b4)
{
	return uint32_t(b1) | (b2 << 8) | (b3 << 16) | (b4 << 24);
}

inline uint32_t MAKEWORD(uint16_t w1, uint16_t w2)
{
	return uint32_t(SWAPBYTES(w1)) | (SWAPBYTES(w2) << 16);
}

struct __attribute__((packed)) RGB {
	uint8_t r, g, b;
};

static_assert(sizeof(RGB) == 3, "RGB not packed!");

class Adafruit_ILI9341 : public Adafruit_GFX, public HSPI::Device
{
private:
	void transmitCmdData(uint8_t cmd, const void* data, size_t size);
	void transmitData(uint16_t data);
	void transmitCmdData(uint8_t cmd, uint32_t data);
	void transmitData(uint16_t data, unsigned repeats);
	void transmitCmd(uint8_t cmd);
	void receiveCmdData(uint8_t cmd, void* buffer, size_t bufSize);

	HSPI::IoModes getSupportedIoModes() const override
	{
		return HSPI::IoMode::SPI;
	}

public:
	Adafruit_ILI9341(HSPI::Controller& spi) : Adafruit_GFX(ILI9341_TFTWIDTH, ILI9341_TFTHEIGHT), Device(spi)
	{
	}

	bool begin(HSPI::PinSet pinSet, uint8_t chipSelect);

	// Adafruit_GFX
	void fillScreen(uint16_t color) override;
	void pushColor(uint16_t color) override;
	void pushColors(uint16_t* colors, size_t numColors) override;
	void drawPixel(int16_t x, int16_t y, uint16_t color) override;
	void drawFastVLine(int16_t x, int16_t y, int16_t h, uint16_t color) override;
	void drawFastHLine(int16_t x, int16_t y, int16_t w, uint16_t color) override;
	void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) override;

	void setRotation(uint8_t r);
	void invertDisplay(bool i);
	void setAddrWindow(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1);

	// Pass 8-bit (each) R,G,B, get back 16-bit packed color
	static uint16_t pack565(const RGB& col)
	{
		return ((col.r & 0xF8) << 8) | ((col.g & 0xFC) << 3) | (col.b >> 3);
	}

	static RGB unpack565(uint16_t col)
	{
		return RGB{uint8_t((col >> 8) & 0xF8), uint8_t((col >> 3) & 0xFC), uint8_t(col << 3)};
	}

	static uint16_t color565(uint8_t r, uint8_t g, uint8_t b)
	{
		return pack565(RGB{r, g, b});
	}

	void writeMem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, const void* data, size_t size);
	void readMem(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, void* buffer, size_t size);

	uint32_t readRegister(uint8_t cmd, uint8_t byteCount);

	uint32_t readDisplayId()
	{
		return readRegister(ILI9341_RDDID, 3);
	}

	uint32_t readDisplayStatus()
	{
		return readRegister(ILI9341_RDDST, 4);
	}

	uint8_t readPowerMode()
	{
		return readRegister(ILI9341_RDMODE, 1);
	}

	uint8_t readMADCTL()
	{
		return readRegister(ILI9341_RDMADCTL, 1);
	}

	uint8_t readPixelFormat()
	{
		return readRegister(ILI9341_RDPIXFMT, 1);
	}

	uint8_t readImageFormat()
	{
		return readRegister(ILI9341_RDIMGFMT, 1);
	}

	uint8_t readSignalMode()
	{
		return readRegister(ILI9341_RDSIGMODE, 1);
	}

	uint8_t readSelfDiag()
	{
		return readRegister(ILI9341_RDSELFDIAG, 1);
	}
};

#endif
