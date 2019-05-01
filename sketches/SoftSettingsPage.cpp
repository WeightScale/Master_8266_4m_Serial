#include "SoftSettingsPage.h"
#include "Board.h"
#include "BrowserServer.h"

SoftSettingsPageClass * SoftSettingsPage;

bool SoftSettingsPageClass::canHandle(AsyncWebServerRequest *request) {	
	if (request->url().equalsIgnoreCase(F("/soft.html"))) {
		if (!request->authenticate(_value->user, _value->password)) {
			if (!server->checkAdminAuth(request)) {
				request->requestAuthentication();
				return false;
			}
		}
		return true;
	}else
		return false;
}

void SoftSettingsPageClass::handleRequest(AsyncWebServerRequest *request) {
	if (request->args() > 0) {
		String message = " ";
		DynamicJsonBuffer jsonBuffer;
		JsonObject &json = jsonBuffer.createObject();
		if (request->hasArg("host")){	
			int args = request->args();	
			json["cmd"] = "snet";
			for (int i = 0; i < args; i++){
				json[request->argName(i)] = request->arg(i);				
			}
			if (request->hasArg("dnip"))
				json["dnip"] = true;
			else
				json["dnip"] = false;
			json.printTo(message);
			Serial.println(message);			
		}else if (request->hasArg("server")){
			json["cmd"] = "server";
			json["server"] = request->arg("server");
			json["key"] = request->arg("key").toInt();
			json.printTo(message);
			Serial.println(message);
		}
	}
#ifdef HTML_PROGMEM
	request->send_P(200, F("text/html"), soft_html);
#else
	request->send(SPIFFS, request->url());
#endif
}

void SoftSettingsPageClass::handleValue(AsyncWebServerRequest * request) {
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
