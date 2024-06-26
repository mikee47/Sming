/**
 * WARNING !
 * Ultrasonic_HCSR04 modules usually work with 5v power and TTL levels,
 * so you NEED voltage divider or level shifter for ECHO pin.
 *
 * Trigger pin is tolerant to 3.3v and should work without problems
 *
 * By nik.sharky http://esp8266.ru/forum/members/sharky.396/
 */

#include <SmingCore.h>
#include <Libraries/Ultrasonic/Ultrasonic.h>

#define TRIG_PIN 2
#define ECHO_PIN 5

namespace
{
SimpleTimer procTimer;
Ultrasonic ultrasonic;

void measure()
{
	// get distance
	uint16_t dist = ultrasonic.rangeCM();

	// print the distance
	Serial.println(dist);
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	ultrasonic.begin(TRIG_PIN, ECHO_PIN);
	procTimer.initializeMs<500>(measure).start();
}
