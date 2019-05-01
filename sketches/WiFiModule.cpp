#include "WiFiModule.h"

IPAddress apIP(192, 168, 4, 1);
IPAddress netMsk(255, 255, 255, 0);

WiFiModuleClass::WiFiModuleClass(char *host) : Task(5000) {
	//onRun(std::bind(&WiFiModuleClass::scan, this));
	_hostName = String(host);	
	WiFi.persistent(false);
	WiFi.setAutoConnect(false);
	WiFi.setAutoReconnect(false);
	
#ifdef DEBUG_CLIENT
	WiFi.mode(WIFI_AP_STA);
#else
	WiFi.mode(WIFI_AP);
#endif // DEBUG_CLIENT		
	WiFi.softAPConfig(apIP, apIP, netMsk);
	WiFi.softAP(_hostName.c_str(), "23232323");
	_hostName.toLowerCase();
	WiFi.hostname(_hostName);
};

#ifdef DEBUG_CLIENT
	wl_status_t WiFiModuleClass::connect() {
	wl_status_t _status = WiFi.status();	
	
	if (_status == WL_DISCONNECTED || _status == WL_NO_SSID_AVAIL || _status == WL_IDLE_STATUS || _status == WL_CONNECT_FAILED) {
			
		WiFi.begin("KONST", "3fal-rshc-nuo3");
		_status = WiFi.status();
		static const uint32_t connectTimeout = 5000;      //5s timeout
		auto startTime = millis();
		// wait for connection, fail, or timeout
		while(_status != WL_CONNECTED && _status != WL_NO_SSID_AVAIL && _status != WL_CONNECT_FAILED && (millis() - startTime) <= connectTimeout) {
			delay(10);
			_status = WiFi.status();
		}
	}
	return _status;	
};			  
#endif // DEBUG_CLIENT

String /*ICACHE_RAM_ATTR*/ WiFiModuleClass::readSerial(uint32_t timeout) {	
	String tempData = "";
	uint64_t timeOld = millis();
	while (Serial.available() || (millis() < (timeOld + timeout))) {
		if (Serial.available()) {	
			tempData += (char) Serial.read();
			timeOld = millis();
		}
		yield();
	}
	return tempData;
}