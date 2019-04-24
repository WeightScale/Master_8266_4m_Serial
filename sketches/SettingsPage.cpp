#include "SettingsPage.h"
#include "Board.h"

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
		if (!request->authenticate(MASTER_USER, MASTER_PASS)) {
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
			request->arg("n_admin").toCharArray(_value->user, request->arg("n_admin").length() + 1);
			request->arg("p_admin").toCharArray(_value->password, request->arg("p_admin").length() + 1);
			if (Board->memory()->save()) {
				goto url;
			}
		}		
		return request->send(400);
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