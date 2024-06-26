#include <SmingCore.h>
#include <Adafruit_VL53L0X.h>

// GPIO - NodeMCU pins
#define SDA 4	// D2
#define SCL 5	// D1
#define XSHUT 14 // D5
#define INT 12   // D6

namespace
{
Adafruit_VL53L0X lox;
SimpleTimer loopTimer;

void loop()
{
	VL53L0X_RangingMeasurementData_t measure;

	Serial.print(_F("Reading a measurement... "));
	lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!

	if(measure.RangeStatus != 4) { // phase failures have incorrect data
		Serial << _F("Distance: ") << measure.RangeMilliMeter << " mm" << endl;
	} else {
		Serial.println(_F("Out of range"));
	}
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	Wire.begin(SDA, SCL);

	if(!lox.begin(VL53L0X_I2C_ADDR, false, &Wire, Adafruit_VL53L0X::VL53L0X_SENSE_LONG_RANGE)) {
		Serial.println(_F("Failed to boot VL53L0X"));
		return;
	}

	Serial.println(_F("VL53L0X API Simple Ranging example\n\n"));
	loopTimer.initializeMs<100>(loop).start();
}
