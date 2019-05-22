#include "SoftSettingsPage.h"
#include "Board.h"
#include "BrowserServer.h"

SoftSettingsPageClass * SoftSettingsPage;

bool SoftSettingsPageClass::canHandle(AsyncWebServerRequest *request) {	
#ifdef MULTI_POINTS_CONNECT
	if (request->url().equalsIgnoreCase(F("/soft.html"))) {
#else
	if (request->url().equalsIgnoreCase(F("/soft.html"))) {
#endif // MULTI_POINTS_CONNECT	
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
#ifdef MULTI_POINTS_CONNECT	
		if (request->hasArg("delete")) {
			json["cmd"] = "delpoint";
			json["ssid"] = request->arg("ssid");				
		}
		else if (request->hasArg("ssid")) {
			json["cmd"] = "point";
			json["ssid"] = request->arg("ssid");
			json["key"] = request->arg("key");
			if (request->hasArg("dnip"))
				json["dnip"] = true;
			else
				json["dnip"] = false;	
			json["lan_ip"] = request->arg("lan_ip");
			json["gateway"] = request->arg("gateway");
			json["subnet"] = request->arg("subnet");
		}
#else
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
#endif // MULTI_POINTS_CONNECT			
		json.printTo(message);				
		Serial.println(message);
	}
#ifdef HTML_PROGMEM
	
	#ifdef MULTI_POINTS_CONNECT
		request->send_P(200, F("text/html"), softm_html);
	#else
		request->send_P(200, F("text/html"), soft_html);
	#endif // MULTI_POINTS_CONNECT
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
