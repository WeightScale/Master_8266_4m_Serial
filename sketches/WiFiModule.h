#pragma once
#include "Task.h"
#include <ESP8266WiFi.h>

//#define DEBUG_CLIENT

class WiFiModuleClass : /*public ESP8266WiFiClass,*/ public Task {
private:
	String _hostName;
public:
	WiFiModuleClass(char *host);
#ifdef DEBUG_CLIENT
	wl_status_t connect();			  
#endif // DEBUG_CLIENT
	String readSerial(uint32_t timeou = 5);
};

extern IPAddress apIP;
extern IPAddress netMsk;