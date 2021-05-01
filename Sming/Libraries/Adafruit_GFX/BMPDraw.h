/***************************************************
  This is a library for the Adafruit 1.8" SPI display.

This library works with the Adafruit 1.8" TFT Breakout w/SD card
  ----> http://www.adafruit.com/products/358
The 1.8" TFT shield
  ----> https://www.adafruit.com/product/802
The 1.44" TFT breakout
  ----> https://www.adafruit.com/product/2088
as well as Adafruit raw 1.8" TFT display
  ----> http://www.adafruit.com/products/618

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/
/****************************************************
 * ported for Sming by H.Boettcher.
 * this implementation uses SPIFFS storage instead of SD cards
 * hbottc@gmail.com
 ***************************************************/
/*
 * Revised as a template class so it can be used with any supported Adafruit display library,
 * such as Adafruit_ILI9341, Adafruit_ST7735, etc.
 * June 2019 mikee47 <mikee@sillyhouse.net>
 */

#pragma once

#include "Arduino.h"

/*
 * This function opens a Windows Bitmap (BMP) file and displays it at the given coordinates.
 * It's sped up by reading many pixels worth of data at a time (rather than pixel by pixel).
 * Increasing the buffer size takes more of the esp8266's precious RAM but makes loading a
 * little faster.  20 pixels seems a good balance.
 *
 * The `Adafruit_TFT` class must provide these methods:
 *
 *		unsigned width()
 * 		unsigned height()
 * 		setAddrWindow(x1, y1, x2, y2)
 * 		pushColor(uint16_t rgb565)
 *
 * Return types and parameters don't need to be exact.
 * Note that this would probably be more efficient using a virtual base class...
*/

#define BUFFPIXEL 20

template <class Adafruit_TFT> bool bmpDraw(Adafruit_TFT& tft, IDataSourceStream& stream, uint8_t x, uint8_t y)
{
	if((x >= tft.width()) || (y >= tft.height())) {
		return false;
	}

	debug_d("Loading image");

	uint32_t startTime = millis();

	// These read 16- and 32-bit types from the SPIFFS file.
	// BMP data is stored little-endian, esp8266 is little-endian too.
	// May need to reverse subscript order if porting elsewhere.

	uint32_t streamPos{0};

	auto read16 = [&]() -> uint16_t {
		char bytes[2];
		streamPos += stream.readBytes(bytes, 2);
		return (bytes[1] << 8) + bytes[0];
	};

	auto read32 = [&]() -> uint32_t {
		char bytes[4];
		streamPos += stream.readBytes(bytes, 4);
		return (bytes[3] << 24) + (bytes[2] << 16) + (bytes[1] << 8) + bytes[0];
	};

	// Convert rgb values into RGB565 format
	auto color565 = [](uint8_t r, uint8_t g, uint8_t b) -> uint16_t {
		return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
	};

	// Parse BMP header
	bool goodBmp = false;
	while(true) {
		// BMP signature
		if(read16() != 0x4D42) {
			debug_e("[BMP] Invalid signature");
			break;
		}

		// Get File Size
		size_t fileSize = read32();
		(void)fileSize;
		debug_d("[BMP] File size: %u", fileSize);

		// Read & ignore creator bytes
		(void)read32();

		// Start of image data
		uint32_t bmpImageoffset = read32();
		debug_d("[BMP] Image Offset: %u", bmpImageoffset);

		// Read DIB header
		uint32_t dibHeaderSize = read32();
		(void)dibHeaderSize;
		debug_d("[BMP] Header size: %u", dibHeaderSize);

		int bmpWidth = read32();
		int bmpHeight = read32();
		if(read16() != 1) { // # planes -- must be '1'
			debug_e("[BMP] Un-supported planes");
			break;
		}

		uint8_t bmpDepth = read16();			  // bits per pixel
		if((bmpDepth != 24) || (read32() != 0)) { // 0 = uncompressed
			debug_e("[BMP] Un-supported depth %u", bmpDepth);
			break;
		}

		// Supported BMP format -- proceed!
		goodBmp = true;

		debug_d("[BMP] Image size %u x %u", bmpWidth, bmpHeight);

		// BMP rows are padded (if needed) to 4-byte boundary
		uint32_t rowSize = ALIGNUP4(bmpWidth * 3);

		// If bmpHeight is negative, image is in top-down order.
		// This is not canon but has been observed in the wild.
		bool flip = true; // BMP is stored bottom-to-top
		if(bmpHeight < 0) {
			bmpHeight = -bmpHeight;
			flip = false;
		}

		// Crop area to be loaded
		int w = bmpWidth;
		int h = bmpHeight;
		if((x + w - 1) >= tft.width()) {
			w = tft.width() - x;
		}
		if((y + h - 1) >= tft.height()) {
			h = tft.height() - y;
		}

		// Set TFT address window to clipped image bounds
		tft.setAddrWindow(x, y, x + w - 1, y + h - 1);

		uint8_t sdbuffer[3 * BUFFPIXEL];	// pixel buffer (R+G+B per pixel)
		uint8_t buffidx = sizeof(sdbuffer); // Current position in sdbuffer
		for(int row = 0; row < h; row++) {  // For each scanline...
			/*
			 * Seek to start of scan line.  It might seem inefficient to be doing this on
			 * every line, but this method covers a lot of gritty details like cropping
			 * and scanline padding.  Also, the seek only takes place if the file position
			 * actually needs to change (avoids a lot of cluster math in SD library).
			 */
			uint32_t pos;
			if(flip) {
				// Bitmap is stored bottom-to-top order (normal BMP)
				pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
			} else {
				// Bitmap is stored top-to-bottom
				pos = bmpImageoffset + row * rowSize;
			}
			if(streamPos != pos) {
				streamPos = stream.seekFrom(pos, SeekOrigin::Start);
				buffidx = sizeof(sdbuffer); // Force buffer reload
			}
			uint16_t lineBuffer[w];
			for(int col = 0; col < w; col++) { // For each pixel...
				// Time to read more pixel data?
				if(buffidx >= sizeof(sdbuffer)) { // Indeed
					streamPos += stream.readBytes(reinterpret_cast<char*>(sdbuffer), sizeof(sdbuffer));
					buffidx = 0; // Set index to beginning
				}

				// Convert pixel from BMP to TFT format, push to display
				uint8_t b = sdbuffer[buffidx++];
				uint8_t g = sdbuffer[buffidx++];
				uint8_t r = sdbuffer[buffidx++];
				lineBuffer[col] = color565(r, g, b);
			} // end pixel
			tft.pushColors(lineBuffer, w);
		} // end scanline
		debug_d("[BMP] Loaded in %d ms", millis() - startTime);
		break;
	}

	if(!goodBmp) {
		debug_e("[BMP] Format not recognized");
	}

	return goodBmp;
}

template <class Adafruit_TFT> bool bmpDraw(Adafruit_TFT& tft, String fileName, uint8_t x, uint8_t y)
{
	FileStream fs;
	if(!fs.open(fileName)) {
		debug_e("[BMP] Error opening file '%s': %s", fileName.c_str(), fs.getLastErrorString().c_str());
		return false;
	}

	debug_d("[BMP] Loading image '%s'", fileName.c_str());
	return bmpDraw(tft, fs, x, y);
}
