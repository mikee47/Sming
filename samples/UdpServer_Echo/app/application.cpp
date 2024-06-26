#include <SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put your SSID and password here
#define WIFI_PWD "PleaseEnterPass"
#endif

namespace
{
void onReceive(UdpConnection& connection, char* data, int size, IpAddress remoteIP, uint16_t remotePort);

// UDP server
const uint16_t EchoPort = 1234;
UdpConnection udp(onReceive);

void onReceive(UdpConnection& connection, char* data, int size, IpAddress remoteIP, uint16_t remotePort)
{
	debugf("UDP Server callback from %s:%d, %d bytes", remoteIP.toString().c_str(), remotePort, size);

	// We implement string mode server for example
	Serial << ">\t" << data;

	// Send echo to remote sender
	String text = F("echo: ") + data;
	udp.sendStringTo(remoteIP, remotePort, text);
}

void gotIP(IpAddress ip, IpAddress gateway, IpAddress netmask)
{
	udp.listen(EchoPort);

	Serial.println(_F("\r\n"
					  "=== UDP SERVER STARTED ==="));
	Serial << WifiStation.getIP() << ':' << EchoPort << endl;
	Serial.println(_F("==========================\r\n"));
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true);

	WifiStation.config(WIFI_SSID, WIFI_PWD);
	WifiEvents.onStationGotIP(gotIP);
}
