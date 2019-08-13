#pragma once
#include "Task.h"
#include <ESP8266WiFi.h>

#define KEY_AP "23232323"

//#define DEBUG_CLIENT

class WiFiModuleClass : public Task {
private:
	String _hostName;
public:
	WiFiModuleClass(char *host);
#ifdef DEBUG_CLIENT
	wl_status_t connect();			  
#endif // DEBUG_CLIENT	
	String hostName() {return _hostName;};
};

extern IPAddress apIP;
extern IPAddress netMsk;