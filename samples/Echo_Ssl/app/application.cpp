/**
 *
 * This example talks to SSL enabled TCP server. If you do not have such you can use ncat (from the nmap package) and run something like this
 *
 * ncat -vvv  -l 0.0.0.0 4444 --ssl --ssl-key ~/Private/x.key --ssl-cert ~/Private/x.cert
 *
 */

#include <SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
#define WIFI_SSID "PleaseEnterSSID" // Put your SSID and password here
#define WIFI_PWD "PleaseEnterPass"
#endif

#ifndef SERVER_IP
// Make sure to define the server IP in the code"
#define SERVER_IP "127.0.0.1"
#endif

namespace
{
SimpleTimer procTimer;
TcpClient* client;

bool showMeta = true;

bool onReceive(TcpClient& tcpClient, char* data, int size)
{
	debugf("Got data with size: %d", size);
	debugf("Free heap: %d", system_get_free_heap_size());
	if(size < 1) {
		return false;
	}

	if(showMeta) {
		auto ssl = tcpClient.getSsl();
		if(ssl != nullptr) {
			ssl->printTo(Serial);
		}
		debugf("end of meta...");
		showMeta = false;
	}

	debugf("Ready to send back the data...");

	return tcpClient.send(data, size);
}

void connectFail(const String& ssid, MacAddress bssid, WifiDisconnectReason reason)
{
	debugf("I'm NOT CONNECTED!");
}

void gotIP(IpAddress ip, IpAddress netmask, IpAddress gateway)
{
	Serial << _F("Got IP ") << ip << endl;
	client = new TcpClient(TcpClientDataDelegate(onReceive));
	client->setSslInitHandler([](Ssl::Session& session) { session.options.verifyLater = true; });
	client->connect(IpAddress(SERVER_IP), 4444, true);
}

} // namespace

void init()
{
	Serial.begin(SERIAL_BAUD_RATE);
	Serial.systemDebugOutput(true); // Allow debug print to serial

	// Setup the WIFI connection
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD);

	// Run our method when station was connected to AP (or not connected)
	WifiEvents.onStationDisconnect(connectFail);
	WifiEvents.onStationGotIP(gotIP);
}
