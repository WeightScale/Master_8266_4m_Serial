#pragma once
#include <Arduino.h>
#include <ESPAsyncWebServer.h>
#include <ESPAsyncDNSServer.h>
#include <ArduinoJson.h>
#include "Config.h"

#define MAX_WEBSOCKET_CLIENT		4

// DNS server
#define DNS_PORT 53

typedef struct {
	String wwwUsername;
	String wwwPassword;
} strHTTPAuth_t;

class BrowserServerClass : public AsyncWebServer{
	protected:
		strHTTPAuth_t _httpAuth;		
		char * _username;
		char * _password;
	public:
	
	BrowserServerClass(uint16_t port, char * username, char * password);
		~BrowserServerClass();
		void begin();
		void init();
		void send_wwwauth_configuration_html(AsyncWebServerRequest *request);
		String getContentType(String filename);
		bool checkAdminAuth(AsyncWebServerRequest * request);
		bool isAuthentified(AsyncWebServerRequest * request);
		String getName(){ return _httpAuth.wwwUsername;};
		String getPass(){ return _httpAuth.wwwPassword;};
		void stop(){_server.end();};
};

class CaptiveRequestHandler : public AsyncWebHandler {
	public:
	CaptiveRequestHandler() {}
	virtual ~CaptiveRequestHandler() {}
	
	bool canHandle(AsyncWebServerRequest *request){
		if (!request->host().equalsIgnoreCase(WiFi.softAPIP().toString())){
			return true;
		}
		return false;
	}

	void handleRequest(AsyncWebServerRequest *request) {
		AsyncWebServerResponse *response = request->beginResponse(302, "text/plain","");
		response->addHeader("Location", String("http://") + WiFi.softAPIP().toString());
		request->send ( response);
	}
};


class TapeRequestHandler : public AsyncWebHandler {
	public:
	TapeRequestHandler() {}
	virtual ~TapeRequestHandler() {}
	
	virtual bool canHandle(AsyncWebServerRequest *request) override final;
	void handleRequest(AsyncWebServerRequest *request) override final {
		request->send(204, "text/html", "");
	}
};

extern AsyncDNSServer dnsServer;
extern BrowserServerClass  *server;
extern AsyncWebSocket webSocket;

#ifdef HTML_PROGMEM
	void handleBatteryPng(AsyncWebServerRequest*);
	void handleScalesPng(AsyncWebServerRequest*);
#endif
//size_t commandWeight(JsonObject& json);
void handleSettings(AsyncWebServerRequest * request);
//void handleAccessPoint(AsyncWebServerRequest*);
void handleScaleProp(AsyncWebServerRequest*);
void handleRSSI(AsyncWebServerRequest*);
void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
void onSsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);