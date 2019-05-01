#pragma once
#include <ESPAsyncWebServer.h>
#include "Config.h"
#include "Scales.h"

#define WEIGHT_MAX_JSON		"mw_id"
#define OFFSET_JSON			"ofs"
#define AVERAGE_JSON		"av_id"
#define STEP_JSON			"st_id"
#define ACCURACY_JSON		"ac_id"
#define SCALE_JSON			"scale"
#define FILTER_JSON			"fl_id"
#define SEAL_JSON			"sl_id"
#define USER_JSON			"us_id"
#define PASS_JSON			"ps_id"
#define RATE_JSON			"rt_id"

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
	void handleValue(AsyncWebServerRequest * request);
	size_t doCalibrateValue(JsonObject& root);
};

extern CalibratePageClass * CalibratePage;

