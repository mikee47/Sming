/*
 * For testing time functions: sunrise/sunset, etc.
 *
 */

#include "user_config.h"

#include <except.h>
#include <SolarCalculator.h>
#include <time.h>
#include <stdio.h>

#include <Services/WebHelpers/base64.h>
#include <Timezone.h>

#include "v4test.h"

#include "Data/Stream/ReadWriteStream.h"

class TTYStream : public ReadWriteStream
{
    /**
     * @brief Return the total length of the stream
     * @retval int -1 is returned when the size cannot be determined
    */
    int available() override
    {
      return -1;
    }

    using ReadWriteStream::write;

    /** @brief  Write chars to end of stream
       *  @param  buffer Pointer to buffer to write to the stream
       *  @param  size Quantity of chars to write
       *  @retval size_t Quantity of chars written to stream
       */
    size_t write(const uint8_t* buffer, size_t size) override
    {
      return m_nputs(reinterpret_cast<const char*>(buffer), size);
    }

    //Use base class documentation
    uint16_t readMemoryBlock(char* data, int bufSize) override
    {
      return 0;
    }

    //Use base class documentation
    bool seek(int len) override
    {
      return false;
    }

    //Use base class documentation
    bool isFinished() override
    {
      return true;
    }
};

TTYStream Serial;


void base64test()
{
	unsigned char test[] = "This is a text string to test base64 encoding. Not very scientific...";

	char b64buf[1024];
	int b64len = base64_encode(sizeof(test), test, sizeof(b64buf), b64buf);
	debug_i("base64_encode(%u, *, %u, *) returned %d", sizeof(test), sizeof(b64buf), b64len);
	debug_hex(DBG, "b64", b64buf, b64len);

	char buf[1024];
	int ret = base64_decode(b64len, b64buf, sizeof(buf), (uint8_t*)buf);
	debug_i("base64_decode(%u, *, %u, *) returned %d", b64len, sizeof(buf), ret);

	debug_i("%s", buf);

	if(ret == sizeof(test) && memcmp(buf, test, ret) == 0)
		debug_i("All OK");
	else
		debug_e("FAIL");

	//
	strcpy(b64buf, "iUKOZ0raxgHlcYFqEgm+WIxyC28=");
	b64len = strlen(b64buf);
	ret = base64_decode(b64len, b64buf, sizeof(buf), (uint8_t*)buf);
	debug_i("base64_decode(%u, *, %u, *) returned %d", b64len, sizeof(buf), ret);
	debug_hex(DBG, "buf", buf, ret);

	b64len = base64_encode(ret, (uint8_t*)buf, sizeof(b64buf), b64buf);
	debug_i("base64_encode(%u, *, %u, *) returned %d", sizeof(test), sizeof(b64buf), b64len);
	debug_hex(DBG, "b64", b64buf, b64len);
}

static size_t __nputs(const char* str, size_t len)
{
  return fwrite(str, 1, len, stdout);
}

int main(int argc, char* argv[])
{
  trap_exceptions();

  m_setPuts(__nputs);

	//  char test[] = "!";
	//  debug_i("sizeof(test) = %d", sizeof(test));

//	base64test();

//	testDateTime();

//	testUrl(Serial);

//  testObjectMap();

  testSerialBuffer();

	return 0;

	time_t timestamp = time(nullptr);

	debug_i("%s(%u)", __FUNCTION__, (int)timestamp);

	// United Kingdom (London, Belfast)
	TimeChangeRule BST = {Last, Sun, Mar, 1, 60};
	TimeChangeRule GMT = {Last, Sun, Oct, 2, 0};
	Timezone daylight(BST, GMT);

	time_t local = daylight.toLocal(timestamp);
	debug_i("Local = %d", (int)local);

	float diff = (local - timestamp) / SECS_PER_HOUR;
	debug_i("TZ diff = %f", diff);

	// Location co-ordinates should be part of config
	const float lat = 52.067;  // 52.01486;
	const float lng = -0.7867; // -0.70126;
	//

	SolarCalculator mk({lat, lng});

	DateTime dt(timestamp);
	int sunrise = mk.sunrise(dt.Year, dt.Month + 1, dt.Day);
	debug_i("Sunrise: %s", DateTime(sunrise * SECS_PER_MIN).toShortTimeString().c_str());

	int sunset = mk.sunset(dt.Year, dt.Month + 1, dt.Day);
	debug_i("Sunset: %s", DateTime(sunset * SECS_PER_MIN).toShortTimeString().c_str());

	{
		SolarCalculator losAngeles({34.0522, -118.2437});
		SolarCalculator antarctica({-77.85, 166.6667});

		// 14:58
		int laSunrise = losAngeles.sunrise(2017, 12, 31);
		debug_i("LA Sunrise: %s", DateTime(laSunrise * SECS_PER_MIN).toShortTimeString().c_str());

		// 00:54, 1st Jan
		int laSunset = losAngeles.sunset(2017, 12, 31);
		debug_i("LA Sunset: %s", DateTime(laSunset * SECS_PER_MIN).toShortTimeString().c_str());

		int antSunrise = antarctica.sunrise(2017, 6, 30);
		debug_i("Antarctica Sunrise: %d", (int)antSunrise);
	}

	return 0;
}
