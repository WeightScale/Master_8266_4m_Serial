#include "CalibratePage.h"
#include "Board.h"

CalibratePageClass::CalibratePageClass(t_scales_value * value) {};

bool CalibratePageClass::canHandle(AsyncWebServerRequest *request) {	
	if (request->url().equalsIgnoreCase(F("/calibr.html"))) {
		if (!request->authenticate(_value->user, _value->password)) {
			if (!request->authenticate(MASTER_PASS, MASTER_PASS)) {
				request->requestAuthentication();
				return false;
			}
		}
		return true;
	}else
		return false;
}

void CalibratePageClass::handleRequest(AsyncWebServerRequest *request) {
	if (request->args() > 0) {
		if (request->hasArg("update")) {
			_value->accuracy = request->arg(F("weightAccuracy")).toInt();
			_value->step = request->arg(F("weightStep")).toInt();
			Board->scales()->average(request->arg(F("weightAverage")).toInt());
			Board->scales()->SetFilterWeight(request->arg(F("weightFilter")).toInt());
			_value->filter = Board->scales()->GetFilterWeight();
			_value->max = request->arg(F("weightMax")).toFloat();
			if (request->hasArg("rate"))
				_value->rate = true;
			else
				_value->rate = false;
			digitalWrite(RATE, _value->rate);
			Board->scales()->mathRound();
			if (saveValue()) {
				goto ok;
			}
			goto err;
		}
		
		if (request->hasArg("zero")) {
			Board->scales()->tare();
		}
		
		if (request->hasArg(F("weightCal"))) {
			float rw = request->arg(F("weightCal")).toFloat();
			Board->scales()->SetCurrent(Board->scales()->read());
			long cw = Board->scales()->Current();
			//long cw = readAverage();
			mathScale(rw, cw);
		}
		
		if (request->hasArg("user")) {
			request->arg("user").toCharArray(_value->user, request->arg("user").length() + 1);
			request->arg("pass").toCharArray(_value->password, request->arg("pass").length() + 1);
			if (saveValue()) {
				goto url;
			}
			goto err;
		}
		
ok:
		return request->send(200, F("text/html"), "");
err:
		return request->send(204);	
	}
url:
#ifdef HTML_PROGMEM
	request->send_P(200, F("text/html"), calibr_html);
#else
	request->send(SPIFFS, request->url());
#endif
}

bool CalibratePageClass::saveValue() {
	return Board->memory()->save();
};