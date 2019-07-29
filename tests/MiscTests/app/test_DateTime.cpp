/*
 * test_DateTime.cpp
 *
 * @author: 19 Feb 2019 - mikee47 <mike@sillyhouse.net>
 */

#include <time.h>
#include <system/include/m_printf.h>
#include <WString.h>
#include <DateTime.h>

void showTime(time_t timestamp)
{
	auto print = [](const String& s) { m_nputs(s.c_str(), s.length()); };

	auto println = [](const String& s) {
		m_nputs(s.c_str(), s.length());
		m_putc('\n');
	};

	DateTime dt(timestamp);
	//Non-time
	println(dt.format("%%%% Percent sign: %%"));
	println(dt.format("%%n New-line character: %n"));
	println(dt.format("%%t Horizontal-tab character: >|%t|<"));
	//Year
	println(dt.format("%%Y Full year (YYYY): %Y"));
	println(dt.format("%%C Year, first two digits (00-99)%: %C"));
	println(dt.format("%%y Year, last two digits (00-99): %y"));
	//Month
	println(dt.format("%%B Full month name (e.g. June): %B"));
	println(dt.format("%%b Abbreviated month name (e.g. Jun): %b"));
	println(dt.format("%%h Abbreviated month name (e.g. Jun): %h"));
	println(dt.format("%%m Month as a decimal number (01-12): %m"));
	//Week
	println(dt.format("%%U Week number with the first Sunday as the first day of week one (00-53): %U")); //NYI
	println(dt.format("%%W Week number with the first Monday as the first day of week one (00-53): %W")); //NYI
	println(dt.format("%%V ISO 8601 week number (01-53): %V"));											  //NYI
	//Day
	println(dt.format("%%j Day of the year (001-366): %j"));
	println(dt.format("%%d Day of the month, zero-padded (01-31)%: %d"));
	println(dt.format("%%e Day of the month, space-padded ( 1-31): %e"));
	println(dt.format("%%A Full weekday name (e.g. Monday): %A"));
	println(dt.format("%%a Abbreviated weekday name (e.g. Mon): %a"));
	println(dt.format("%%w Weekday as a decimal number with Sunday as 0 (0-6): %w"));
	println(dt.format("%%u ISO 8601 weekday as number with Monday as 1 (1-7): %u"));
	//Hour
	println(dt.format("%%p Meridiem (AM|PM): %p"));
	println(dt.format("%%H Hour in 24h format (00-23): %H"));
	println(dt.format("%%h Hour in 12h format (01-12): %I"));
	//Minute
	println(dt.format("%%M Minute (00-59): %M"));
	//Second
	println(dt.format("%%S Second (00-61): %S"));
	//Formatted strings
	println(dt.format("%%R 24-hour time (HH:MM): %R"));
	println(dt.format("%%r 12-hour time (hh:MM:SS AM): %r"));
	println(dt.format("%%c Locale date and time: %c"));
	println(dt.format("%%D US short date (MM/DD/YY): %D"));
	println(dt.format("%%F ISO 8601 date (YYYY-MM-DD): %F"));
	println(dt.format("%%T ISO 8601 time (HH:MM:SS): %T"));
	println(dt.format("%%x Locale date: %x"));
	println(dt.format("%%X Locale time: %X"));
	//HTTP date
	print("toHTTPDate: ");
	println(dt.toHTTPDate());
	DateTime dt2;
	dt2.fromHttpDate(dt.toHTTPDate());
	print("fromHTTPDate: ");
	println(dt2.toHTTPDate());
	print("toFullDateTimeString: ");
	println(dt.toFullDateTimeString());
	print("toISO8601: ");
	println(dt.toISO8601());
	print("toShortDateString: ");
	println(dt.toShortDateString());
	print("toShortTimeString: ");
	println(dt.toShortTimeString());
}

void testDateTime()
{
	static DEFINE_FSTR(testDateString, "Sun, 06 Nov 1994 08:49:37 GMT");

	/** @brief  Parse a HTTP full date and set time and date
   *  @param  httpDate HTTP full date in RFC 1123 format, e.g. Sun, 06 Nov 1994 08:49:37 GMT
   *  @retval bool True on success
   *  @note   Also supports obsolete RFC 850 date format, e.g. Sunday, 06-Nov-94 08:49:37 GMT where 2 digit year represents range 1970-2069
   *  @note   GMT suffix is optional and is always assumed / ignored
   */
	//  bool parseHttpDate(const String& httpDate);
	DateTime dt;
	if(!dt.fromHttpDate(testDateString)) {
		debug_e("fromHttpDate failed");
	} else {
		debug_i("parseHttpDate(\"%s\") produced \"%s\"", String(testDateString).c_str(),
				dt.toFullDateTimeString().c_str());
		debug_i("unixTime = %u", dt.toUnixTime());
		dt = dt.toUnixTime();
		debug_i("\"%s\"", dt.toFullDateTimeString().c_str());
	}

	return;

	//    dt = time(NULL);
	dt.setTime(59, 59, 23, 14, 2, 2019);
	debug_i("\n\nNOW:");
	showTime(dt);

	dt.setTime(13, 1, 1, 1, 1, 1);
	debug_i("\n\n13,1,1,1,1,1:");
	showTime(dt);
}
