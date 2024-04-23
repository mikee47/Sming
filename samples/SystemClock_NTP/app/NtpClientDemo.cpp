#include <NtpClientDemo.h>
#include <SystemClock.h>
#include <HardwareSerial.h>
#include <SolarCalculator.h>

namespace
{
/*
 * For handling local/UTC time conversions
 * This is for the UK, amend as required
 */
const TimeChangeRule dstStart{"BST", Last, Sun, Mar, 1, 60};
const TimeChangeRule stdStart{"GMT", Last, Sun, Oct, 2, 0};
Timezone tz(dstStart, stdStart);

/*
 * We use the y/m/d from local time for sunrise/sunset calculations, and the solar calculator
 * returns the time from midnight in UTC for that day. We therefore need to adjust this
 * to account for timezone and daylight savings.
 */
time_t getNextSunriseSet(bool isSunrise)
{
	auto timeNow = SystemClock.now(eTZ_Local);
	DateTime dt(timeNow);
	dt.Hour = 0;
	dt.Minute = 0;
	dt.Second = 0;
	SolarCalculator calc;
	int offset_secs = SECS_PER_MIN * calc.sunRiseSet(isSunrise, dt.Year, dt.Month + 1, dt.Day);

	time_t t = tz.toLocal(dt + offset_secs);

	// If time has already passed, then make it tomorrow
	if(t < timeNow) {
		t = tz.toLocal(dt + offset_secs + SECS_PER_DAY);
	}

	return t;
}

void checkTimeZoneOffset(time_t systemTime)
{
	static time_t nextChange;
	static const TimeChangeRule* rule;

	if(!rule) {
		tz.toLocal(systemTime, &rule);
	} else if(systemTime < nextChange) {
		return;
	}

	SystemClock.setTimeZoneOffset(rule->offset * SECS_PER_MIN);
	nextChange = tz.getNextChange(systemTime, &rule);
}

} // namespace

void NtpClientDemo::ntpResult(NtpClient& client, time_t ntpTime)
{
	/*
	 * Update the system clock and calculate the correct time offset,
	 * accounting for time zone and daylight savings.
	 */
	SystemClock.setTime(ntpTime, eTZ_UTC);

	// Now we've set the clock, we can determine the initial active timezone and maintain the offset
	SystemClock.onCheckTimeZoneOffset(checkTimeZoneOffset);

	/*
	 * Display the new time
	 */
	Serial << _F("ntpClientDemo Callback: ntpTime = ") << ntpTime << ", " << SystemClock.getSystemTimeString(eTZ_UTC)
		   << _F(" UTC, Local time = ") << SystemClock.getSystemTimeString(eTZ_Local) << ' ' << tz.utcTimeTag(ntpTime)
		   << endl;

	/*
	 * Display times of next sunrise and sunset
	 */
	DateTime sunrise = getNextSunriseSet(true);
	DateTime sunset = getNextSunriseSet(false);
	Serial << _F("Next sunrise at ") << sunrise.toShortTimeString() << _F(", sunset at ") << sunset.toShortTimeString()
		   << endl;

	/*
	 * Display points at which daylight savings changes.
	 */
	DateTime dt(ntpTime);

	auto& dstRule = tz.getRule(true);
	DateTime dstDateTime = dstRule(dt.Year);
	Serial << dstRule.tag << _F(" starts ") << dstDateTime.toHTTPDate() << endl;

	auto& stdRule = tz.getRule(false);
	DateTime stdDateTime = stdRule(dt.Year);
	Serial << stdRule.tag << _F(" starts ") << stdDateTime.toHTTPDate() << endl;

	const TimeChangeRule* rule;
	time_t nextChange = tz.getNextChange(dt, &rule);
	Serial << _F("Next change to ") << rule->tag << _F(" is ") << DateTime(nextChange).toHTTPDate() << endl;
}
