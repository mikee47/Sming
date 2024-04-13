#include <SmingCore.h>
#include <HardwareTimer.h>

/**
 * Demonstrate bug switching timer1 from NMI to FRC interrupts.
 * The LED will flash 4 times, then stop.
 * The 'ALIVE' messages continue to indicate system hasn't actually crashed.
 * What happened is that the NMI is still enabled and blocks FRC callbacks.
 */

#define LED_PIN 2

namespace
{
using Timer1TestApi = Timer1Api<TIMER_CLKDIV_16, eHWT_NonMaskable>;

SimpleTimer statusTimer;
bool createMaskedTimer;
const unsigned interval{500};
unsigned ticks;
unsigned count;

void initTimer();

void IRAM_ATTR blink1(void*)
{
	digitalWrite(LED_PIN, count & 1);
	++count;
	if(count % 8 == 0) {
		System.queueCallback(initTimer);
	}
}

// Blink LED at twice normal rate
void IRAM_ATTR blink2(void*)
{
	blink1(nullptr);
	hw_timer1_write(ticks / 2);
}

void initTimer()
{
	ticks = HardwareTimer::Millis::timeToTicks(interval);
	Serial << _F("Initialising timer, source = ") << (createMaskedTimer ? "FRC1" : "NMI") << _F(", Interval = ")
		   << interval << _F("ms, ticks = ") << ticks << endl;
	auto source = createMaskedTimer ? TIMER_FRC1_SOURCE : TIMER_NMI_SOURCE;

	// >>>> This is the call which breaks switching from NMI to FRC
	hw_timer1_detach_interrupt();
	//<<<<

	hw_timer1_attach_interrupt(source, createMaskedTimer ? blink1 : blink2, nullptr);

	hw_timer1_enable(TIMER_CLKDIV_16, TIMER_EDGE_INT, true);
	hw_timer1_write(ticks);

	createMaskedTimer = !createMaskedTimer;
}

} // namespace

void init()
{
	Serial.begin(COM_SPEED_SERIAL);
	pinMode(LED_PIN, OUTPUT);

	statusTimer.initializeMs<1000>([]() { Serial << SystemClock.now() << " ALIVE!" << endl; });
	statusTimer.start();

	initTimer();
}
