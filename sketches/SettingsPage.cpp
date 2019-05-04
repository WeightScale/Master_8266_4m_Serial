#include "SettingsPage.h"
#include "Board.h"
#include "BrowserServer.h"

SettingsPageClass * SettingsPage;

bool SettingsPageClass::canHandle(AsyncWebServerRequest *request) {	
	if (request->url().equalsIgnoreCase(F("/settings.html"))) {
		goto auth;
	}
#ifndef HTML_PROGMEM
	else if (request->url().equalsIgnoreCase("/sn")) {
		goto auth;
	}
#endif
	else
		return false;
auth:
	if (!request->authenticate(_value->user, _value->password)) {
		if (!server->checkAdminAuth(request)) {
			request->requestAuthentication();
			return false;
		}
	}
	return true;
}

void SettingsPageClass::handleRequest(AsyncWebServerRequest *request) {
	if (request->args() > 0) {
		String message = " ";
		if (request->hasArg("assid")) {	
			request->arg("assid").toCharArray(_value->apSSID, request->arg("assid").length() + 1);
			request->arg("nadmin").toCharArray(_value->user, request->arg("nadmin").length() + 1);
			request->arg("padmin").toCharArray(_value->password, request->arg("padmin").length() + 1);
			if (Board->memory()->save())
				goto url;
			else
				return request->send(400);
		}		
		return request->send(204);
	}
url:
#ifdef HTML_PROGMEM
	request->send_P(200, F("text/html"), settings_html);
#else
	if (request->url().equalsIgnoreCase("/sn"))
		request->send_P(200, F("text/html"), netIndex);
	else
		request->send(SPIFFS, request->url());
#endif
}

void SettingsPageClass::handleValue(AsyncWebServerRequest * request) {
	if (!request->authenticate(_value->user, _value->password)) {
		if (!server->checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	}
	
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	Board->doSettings(root);	
	root.printTo(*response);
	request->send(response);
}

void SettingsPageClass::handleProp(AsyncWebServerRequest * request) {
	if (!request->authenticate(_value->user, _value->password)) {
		if (!server->checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	}
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["id_local_host"] = Board->wifi()->hostName();
	//root["id_ap_ssid"] = String(CORE->getApSSID());
	root["id_ap_ip"] = toStringIp(WiFi.softAPIP());
	root["id_slv_ip"] = SlaveScales.url();
	root["id_ip"] = Board->softIp();
	root["sl_id"] = String(Board->scales()->seal());
	root["chip_v"] = String(ESP.getCpuFreqMHz());
	root["id_mac"] = WiFi.macAddress();
	root["id_vr"] = SKETCH_VERSION;
	root.printTo(*response);
	request->send(response);
}