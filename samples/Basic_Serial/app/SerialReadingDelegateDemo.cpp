/*
 * SerialReadingDelegateDemo.cpp
 */

#include "SerialReadingDelegateDemo.h"

//*** Example of global callback routine
void onDataCallback(Stream& stream, char arrivedChar, unsigned short availableCharsCount)
{
	// Note: we're using the global Serial here, but it may not be the same port as stream
	Serial << "Char: " << uint8_t(arrivedChar) << ", Count: " << availableCharsCount << endl;
}

void echoCallback(Stream& stream, char arrivedChar, unsigned short availableCharsCount)
{
	stream.write(arrivedChar);
}

void SerialReadingDelegateDemo::begin(HardwareSerial& serial)
{
	this->serial = &serial;
	serial.onDataReceived(StreamDataReceivedDelegate(&SerialReadingDelegateDemo::onData, this));
	debugf("hwsDelegateDemo instantiated, waiting for data");
}

void SerialReadingDelegateDemo::onData(Stream& stream, char arrivedChar, unsigned short availableCharsCount)
{
	Serial << _F("Class Delegate Demo Time = ") << micros() << _F(" char = 0x") << String(arrivedChar, HEX, 2)
		   << _F(" available = ") << availableCharsCount << endl;

	// Error detection
	unsigned status = serial->getStatus();
	if(status != 0) {
		if(bitRead(status, eSERS_Overflow)) {
			serial->println(_F("** RECEIVE OVERFLOW **"));
		}
		if(bitRead(status, eSERS_BreakDetected)) {
			serial->println(_F("** BREAK DETECTED **"));
		}
		if(bitRead(status, eSERS_FramingError)) {
			serial->println(_F("** FRAMING ERROR **"));
		}
		if(bitRead(status, eSERS_ParityError)) {
			serial->println(_F("** PARITY ERROR **"));
		}
		// Discard what is likely to be garbage
		serial->clear(SERIAL_RX_ONLY);
		return;
	}

	numCallback++;

	/*
	 * A more functional serial command-line interface can be implemented using the `LineBuffer` class.
	 * See `Basic_DateTime` sample application for a demonstration.
	 */

	if(arrivedChar == '\n') // Lets show data!
	{
		serial->println(_F("<New line received>"));
		// Read the string into a line
		String line;
		line.reserve(availableCharsCount);
		while(availableCharsCount--) {
			char cur = stream.read();
			charReceived++;
			serial->print(cur);
			if(cur != '\n' && cur != '\r') {
				line += cur;
			}
		}
		serial->println();

		if(callback) {
			callback(line);
		}
	}
}
