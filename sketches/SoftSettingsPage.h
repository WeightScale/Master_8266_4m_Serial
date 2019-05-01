#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"

class SoftSettingsPageClass : public AsyncWebHandler {
private:
	settings_t * _value;
public:
	SoftSettingsPageClass(settings_t * value) : _value(value) {};
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};	
	void handleValue(AsyncWebServerRequest * request);
};

extern SoftSettingsPageClass * SoftSettingsPage;