#include <SmingCore.h>
#include <Libraries/Adafruit_ILI9341/Adafruit_ILI9341.h>
#include <Libraries/Adafruit_GFX/BMPDraw.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put your SSID and password here
#define WIFI_PWD "PleaseEnterPass"
#endif

/*
Pinout:
MISO GPIO12
MOSI GPIO13
CLK GPIO14
CS GPIO15
DC GPIO5
RST GPIO4
*/

HSPI::Controller spi;
Adafruit_ILI9341 tft(spi);
Timer guiTimer;

static constexpr HSPI::PinSet TFT_PINSET{HSPI::PinSet::overlap};
static constexpr uint8_t TFT_CS{2};
static unsigned state;

class BasicGui
{
public:
	void show();

private:
	int r{0};
	int ara{4};
	int yerara{15};
	int u1{100};
	int u2 = 320 - (u1 + ara);
	int s1{0};
	int s2 = u1 + ara;
	int p1{50};

	int g{28};
	int y{90};
	int satir{6};
};

BasicGui gui;

void basicBitmap()
{
	tft.fillScreen(ILI9341_BLACK);			// Clear display
	tft.setRotation(tft.getRotation() + 1); // Inc rotation 90 degrees
	// Draw 4 images
	for(unsigned i = 0; i < 4; i++) {
		auto x = i * tft.width() / 4;
		auto y = i * tft.height() / 4;
		bmpDraw(tft, F("sming.bmp"), x, y);
	}
}

void BasicGui::show()
{
	PSTR_ARRAY(lists, "abcdef")

	tft.fillScreen(0);
	tft.setTextSize(1);

	tft.setRotation(1);

	tft.setTextSize(3);
	tft.fillRect(s1, 0, u1 * 2, 48, ILI9341_OLIVE);
	tft.setCursor(15, 15);
	tft.println(F("Sming"));
	tft.setTextSize(2);
	tft.fillRect((u1 * 2) + ara, 0, 318 - (u1 * 2), 48, ILI9341_RED);
	for(int a = 0; a < satir; a++) {
		tft.setTextColor(ILI9341_GREEN);
		tft.fillRect(s1, p1, u1, g, ILI9341_DARKCYAN);
		tft.setCursor(s1 + yerara, p1 + 6);
		tft.setTextColor(ILI9341_WHITE);
		tft.println(lists[a]);
		tft.fillRect(s2, p1, u2, g, ILI9341_DARKCYAN);
		tft.setCursor(s2 + yerara, p1 + 6);
		tft.println(r);
		p1 += g + 4;
	}
	p1 = 50;
	r++;
}

void startPage()
{
	tft.fillScreen(0);
	tft.setRotation(1);
	tft.setTextSize(2);

	tft.setTextColor(ILI9341_GREEN);
	tft.setCursor(0, 0);
	tft.setCursor(60, 60);
	tft.println(F("Sming  Framework"));
	tft.setTextColor(ILI9341_WHITE, ILI9341_BLACK);
	tft.setCursor(60, 75);
	tft.println(F("              v1.1"));
	tft.setTextColor(ILI9341_CYAN);
	tft.setCursor(60, 90);
	tft.println(F("ili9340-40C-41 "));
	tft.setCursor(60, 125);
	tft.println(F("M.Bozkurt"));
}

void run()
{
	++state;

	switch(state) {
	case 1:
		startPage();
		break;
	case 2:
		gui.show();
		break;
	case 3:
		basicBitmap();
		break;
	case 4:
	case 5:
		break;
	default:
		state = 0;
	}
}

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Allow debug output to serial

#ifndef DISABLE_WIFI
	//WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiStation.enable(false);
	WifiAccessPoint.enable(false);
#endif

	spiffs_mount();
	Serial.println(F("FileSystem mounted."));

	Serial.println("Display start");
	tft.begin(TFT_PINSET, TFT_CS);

	guiTimer.initializeMs<2000>(run).start();
}
