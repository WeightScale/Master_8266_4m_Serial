#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"

const char netIndex[] PROGMEM = R"(<html lang='en'><meta name='viewport' content='width=device-width, initial-scale=1, maximum-scale=1'/><body><form method='POST'><input name='ssid'><br/><input type='password' name='key'><br/><input type='submit' value='ÑÎÕÐÀÍÈÒÜ'></form></body></html>)";

class SettingsPageClass : public AsyncWebHandler {
private:
	settings_t * _value;
public:
	SettingsPageClass(settings_t * value): _value(value){};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};	
	void handleValue(AsyncWebServerRequest * request);
	void handleProp(AsyncWebServerRequest*);
};

extern SettingsPageClass * SettingsPage;