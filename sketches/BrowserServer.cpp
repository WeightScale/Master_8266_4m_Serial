#include "BrowserServer.h"
#include "SlaveScales.h"
#include <SPIFFSEditor.h>
#include "HttpUpdater.h"
#include "Board.h"
#include <AsyncJson.h>
#include "SettingsPage.h"

//using namespace std::placeholders;

/* */
//ESP8266HTTPUpdateServer httpUpdater;
/* Soft AP network parameters */


//BrowserServerClass server(80);
AsyncWebSocket webSocket("/ws");
//AsyncEventSource events("/events");
AsyncDNSServer dnsServer;

BrowserServerClass::BrowserServerClass(uint16_t port, char * username, char * password)	: AsyncWebServer(port), _username(username), _password(password) {
	_httpAuth.wwwUsername = "sa";
	_httpAuth.wwwPassword = "343434";
}

BrowserServerClass::~BrowserServerClass(){}
	
void BrowserServerClass::begin() {
	/* Setup the DNS server redirecting all the domains to the apIP */
	dnsServer.setTTL(300);
	dnsServer.setErrorReplyCode(AsyncDNSReplyCode::ServerFailure);
	//_downloadHTTPAuth();
	
	webSocket.onEvent(onWsEvent);
	addHandler(&webSocket);
	addHandler(&SlaveScales);
	//addHandler(new TapeRequestHandler());
	//CORE = new CoreClass(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str());	
	//addHandler(CORE);
	addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);
	addHandler(new SPIFFSEditor(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str()));	
	addHandler(new HttpUpdaterClass("sa", "654321"));
	addHandler(SettingsPage);
	init();
	dnsServer.start(DNS_PORT, WiFi.hostname(), apIP);
	AsyncWebServer::begin(); // Web server start
}

void BrowserServerClass::init(){
	on("/wt",HTTP_GET, [](AsyncWebServerRequest * request){					/* Получить вес и заряд. */
		AsyncResponseStream *response = request->beginResponseStream(F("text/json"));
		DynamicJsonBuffer jsonBuffer;
		JsonObject &json = jsonBuffer.createObject();
		JsonObject& master = json.createNestedObject("ms");
		JsonObject& slave = json.createNestedObject("sl");
		
		char b[10];
		float f = Board->scales()->weight() + SlaveScales.weight();
		Board->scales()->formatValue(f, b);
		json["w"] = SlaveScales.isConnected() ? String(b) : String("slave???");
		
		Board->scales()->doData(master);
		Board->battery()->doData(master);
		SlaveScales.doData(slave);
		slave["a"] = Board->scales()->accuracy();
		
		json.printTo(*response);
		request->send(response);			
	});	
	/*on("/wtr",
		HTTP_GET,
		[this](AsyncWebServerRequest * request) {
			/ * Получить вес и заряд. * /
			AsyncResponseStream *response = request->beginResponseStream("text/json");
			DynamicJsonBuffer jsonBuffer;
			JsonObject &json = jsonBuffer.createObject();
			Scale.doDataRandom(json);
			json.printTo(*response);
			request->send(response);
		});*/
	//on("/rc", reconnectWifi);																						/* Пересоединиться по WiFi. */
	on("/settings.json",HTTP_ANY, handleSettings);
	//on("/rc", reconnectWifi);									/* Пересоединиться по WiFi. */
	on("/sv", handleScaleProp);									/* Получить значения. */	
	on("/heap", HTTP_GET, [](AsyncWebServerRequest *request){
		String str = String("Heap: ");
		str += String(ESP.getFreeHeap());
		str += " client: ";
		str += String(webSocket.count());
		request->send(200, F("text/plain"), str);
	});
	on("/rst",HTTP_ANY,[this](AsyncWebServerRequest * request){
		if (!isAuthentified(request)){
			return request->requestAuthentication();
		}
		if(Board->doDefault())
			request->send(200,F("text/html"), F("Установлено!"));
		else
			request->send(400);
	});
	on("/rssi", handleRSSI);
	on("/binfo.html", std::bind(&BoardClass::handleBinfo, Board, std::placeholders::_1));
#ifdef HTML_PROGMEM
	on("/",[](AsyncWebServerRequest * reguest){	reguest->send_P(200, F("text/html"),index_html); });								/* Главная страница. */	 
	on("/global.css",[](AsyncWebServerRequest * reguest){	reguest->send_P(200, F("text/css"), global_css); });					/* Стили */		
	on("/bat.png", handleBatteryPng);
	on("/scales.png",handleScalesPng);	
	on("/und.png",[](AsyncWebServerRequest * request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), und_png, und_png_len) ;
		request->send(response) ;
	});
	on("/set.png",[](AsyncWebServerRequest * request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), set_png, set_png_len);
		request->send(response);
	});
	on("/zerow.png",[](AsyncWebServerRequest * request) {
		AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), zerow_png, zerow_png_len);
		request->send(response);
	});
	serveStatic("/", SPIFFS, "/");
#else
	rewrite("/sn", "/settings.html");
	serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");	
#endif
	if (Board->battery()->isDischarged()) {
		on("/ds",
			[](AsyncWebServerRequest *request) {
				AsyncWebServerResponse *response = request->beginResponse(SPIFFS, "/balert.html");
				response->addHeader(F("Connection"), F("close"));
				request->onDisconnect([]() {
					ESP.deepSleep(20000);
				});
				request->send(response);	
			});
		rewrite("/", "/index.html");
		rewrite("/index.html", "/ds");
	}
	//serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");					
	onNotFound([](AsyncWebServerRequest *request){
		request->send(404);
	});
}

bool TapeRequestHandler::canHandle(AsyncWebServerRequest *request){
	if (request->host().equalsIgnoreCase("/tp")){
		/*if (SlaveScales.doTape()){
			Scale.tare();
			return true;
		}*/
	}
	return false;
}

bool BrowserServerClass::checkAdminAuth(AsyncWebServerRequest * r) {	
	return r->authenticate(_httpAuth.wwwUsername.c_str(), _httpAuth.wwwPassword.c_str());
}

bool BrowserServerClass::isAuthentified(AsyncWebServerRequest * request){
	if (!request->authenticate(_username, _password)){
		if (!checkAdminAuth(request)){
			return false;
		}
	}
	return true;
}

void handleSettings(AsyncWebServerRequest * request){
	if (!server->isAuthentified(request))
		return request->requestAuthentication();
	AsyncResponseStream *response = request->beginResponseStream(F("application/json"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	Board->doSettings(root);	
	root.printTo(*response);
	request->send(response);
}

void handleFileReadAuth(AsyncWebServerRequest * request){
	if (!server->isAuthentified(request)){
		return request->requestAuthentication();
	}
	request->send(SPIFFS, request->url());
}

void handleScaleProp(AsyncWebServerRequest * request){
	/*if (!server->isAuthentified(request))
		return request->requestAuthentication();
	AsyncJsonResponse * response = new AsyncJsonResponse();
	JsonObject& root = response->getRoot();
	root["id_local_host"] = WiFi.hostname();
	root["id_ap_ssid"] = String(CORE->getApSSID());
	root["id_ap_ip"] = toStringIp(WiFi.softAPIP());
	root["id_slv_ip"] = SlaveScales.url();
	root["id_ip"] = toStringIp(WiFi.localIP());
	root["sl_id"] = String(Scale.seal());
	root["chip_v"] = String(ESP.getCpuFreqMHz());
	root["id_mac"] = WiFi.macAddress();
	root["id_vr"] = SKETCH_VERSION;
	response->setLength();
	request->send(response);*/
}

#ifdef HTML_PROGMEM
	void handleBatteryPng(AsyncWebServerRequest * request){
		AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), bat_png, bat_png_len);
		request->send(response);
	}

	void handleScalesPng(AsyncWebServerRequest * request){	
		AsyncWebServerResponse *response = request->beginResponse_P(200, F("image/png"), scales_png, scales_png_len);
		request->send(response);
	}
#endif // HTML_PROGMEM

void handleRSSI(AsyncWebServerRequest * request){
	request->send(200, F("text/html"), String(WiFi.RSSI()));
}

void ICACHE_FLASH_ATTR printScanResult(int networksFound) {
	// sort by RSSI
	int n = networksFound;
	int indices[n];
	int skip[n];
	for (int i = 0; i < networksFound; i++) {
		indices[i] = i;
	}
	for (int i = 0; i < networksFound; i++) {
		for (int j = i + 1; j < networksFound; j++) {
			if (WiFi.RSSI(indices[j]) > WiFi.RSSI(indices[i])) {
				std::swap(indices[i], indices[j]);
				std::swap(skip[i], skip[j]);
			}
		}
	}
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["cmd"] = "ssl";
	JsonArray &scan = root.createNestedArray("list");
	for (int i = 0; i < 5 && i < networksFound; ++i) {
		JsonObject &item = scan.createNestedObject();
		item["ssid"] = WiFi.SSID(indices[i]);
		item["rssi"] = WiFi.RSSI(indices[i]);
	}
	size_t len = root.measureLength();
	AsyncWebSocketMessageBuffer *buffer = webSocket.makeBuffer(len);   //  creates a buffer (len + 1) for you.
	if(buffer) {
		root.printTo((char *)buffer->get(), len + 1);
		webSocket.textAll(buffer);
	}
	WiFi.scanDelete();
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
	if(type == WS_EVT_CONNECT){	
		if (server->count() > MAX_WEBSOCKET_CLIENT) {
			client->text("{\"cmd\":\"cls\",\"code\":1111}");
		}
	}else if(type == WS_EVT_DISCONNECT) {
		//client->close(true);
	}else if(type == WS_EVT_ERROR) {
		client->close(true);
	}else if (type == WS_EVT_DATA) {
		//String msg = "";
		//for(size_t i=0; i < len; i++) {
		//	msg += (char) data[i];
		//}
		DynamicJsonBuffer jsonBuffer(len);
		JsonObject &root = jsonBuffer.parseObject(data);
		if (!root.success()) {
			return;
		}
		const char *command = root["cmd"];			/* Получить показания датчика*/
		JsonObject& json = jsonBuffer.createObject();
		json["cmd"] = command;
		if (strcmp(command, "wt") == 0){
			JsonObject& master = json.createNestedObject("ms");
			JsonObject& slave = json.createNestedObject("sl");
			
			char b[10];
			float f = Board->scales()->weight() + SlaveScales.weight();
			Board->scales()->formatValue(f,b);
			json["w"] = SlaveScales.isConnected() ? String(b) : String("slave???");		
			Board->scales()->doData(master);
			Board->battery()->doData(master);
			SlaveScales.doData(slave);
			slave["a"] = Board->scales()->accuracy();
		}else if (strcmp(command, "tp") == 0){
			#if !defined(DEBUG_WEIGHT_RANDOM)  && !defined(DEBUG_WEIGHT_MILLIS)
				Board->scales()->tare();
				SlaveScales.doTape();
			#endif 
		}else if (strcmp(command, "scan") == 0) {
			WiFi.scanNetworksAsync(printScanResult, true);
			return;
		}else if (strcmp(command, "binfo") == 0) {
			Board->battery()->doInfo(json);
		}else {
			return;
		}
		size_t lengh = json.measureLength();
		AsyncWebSocketMessageBuffer * buffer = webSocket.makeBuffer(lengh);
		if (buffer) {
			json.printTo((char *)buffer->get(), lengh + 1);
			if (client) {
				client->text(buffer);
			}else{
				delete buffer;
			}
		}
	}
}