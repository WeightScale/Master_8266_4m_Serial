#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"
#include "Scales.h"

class CalibratePageClass : public AsyncWebHandler {
private:
	t_scales_value * _value;
public:
	CalibratePageClass(t_scales_value * value);
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	virtual void handleRequest(AsyncWebServerRequest *request) override final;
	virtual bool isRequestHandlerTrivial() override final {return false;};	
	void mathScale(float referenceW, long calibrateW) {
		_value->scale = referenceW / float(calibrateW - _value->offset);
	}
	bool saveValue();
};

