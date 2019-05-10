#include "Board.h"
#include "CalibratePage.h"
#include "SettingsPage.h"
#include "SoftSettingsPage.h"
#include "Battery.h"
#include "SlaveScales.h"
#include "BrowserServer.h"

CalibratePageClass * CalibratePage;
BrowserServerClass * server;

BoardClass::BoardClass() {
	_blink = new BlinkClass();
	_memory = new MemoryClass<MyEEPROMStruct>(&_eeprom);
	if (!_memory->init()) {
		doDefault();
	}
	server = new BrowserServerClass(80, MASTER_USER, MASTER_PASS);
	_battery = new BatteryClass(&_eeprom.settings.bat_min, &_eeprom.settings.bat_max);
	_wifi = new WiFiModuleClass(_eeprom.settings.apSSID);
	_scales = new ScalesClass(DOUT_PIN, SCK_PIN, &_eeprom.scales_value);
	//stationConnected = WiFi.onStationModeConnected(std::bind(&BoardClass::onStationModeConnected, this, std::placeholders::_1));	
	//stationDisconnected = WiFi.onStationModeDisconnected(std::bind(&BoardClass::onStationModeDisconnected, this, std::placeholders::_1));
	//_wifi->loadPoints();
	CalibratePage = new CalibratePageClass(&_eeprom.scales_value);
	SettingsPage = new SettingsPageClass(&_eeprom.settings);
	SoftSettingsPage = new SoftSettingsPageClass(&_eeprom.settings);
#ifdef DEBUG_CLIENT
	_wifi->connect(); 
#endif // DEBUG_CLIENT
};

size_t BoardClass::weightCmd(JsonObject& json) {
	if (_scales->overload()) {
		json["cmd"] = "ovl";
		json["rec"] = "MASTER";
	} else {
		JsonObject& master = json.createNestedObject("ms");
		json["cmd"] = "wt";
		if (SlaveScales.isConnected()) {
			JsonObject& slave = json.createNestedObject("sl");
			SlaveScales.doData(slave);
		}
		_scales->doData(master);
		_battery->doData(master);
	}
	return json.measureLength();
};

size_t BoardClass::weightHttpCmd(JsonObject& json) {
	char b[10];
	JsonObject& master = json.createNestedObject("ms");
	JsonObject& slave = json.createNestedObject("sl");
	float f = Board->scales()->weight() + SlaveScales.weight();
	Board->scales()->formatValue(f, b);
	json["w"] = SlaveScales.isConnected() ? String(b) : String("slave???");		
	_scales->doData(master);
	_battery->doData(master);
	SlaveScales.doData(slave);
	slave["a"] = Board->scales()->accuracy();
	return json.measureLength();
};

bool BoardClass::doDefault() {
	String host = F("MASTER");
	String u = "admin";
	host.toCharArray(_eeprom.settings.apSSID, host.length() + 1);
	u.toCharArray(_eeprom.settings.user, u.length() + 1);
	u.toCharArray(_eeprom.settings.password, u.length() + 1);
	u.toCharArray(_eeprom.scales_value.user, u.length() + 1);
	u.toCharArray(_eeprom.scales_value.password, u.length() + 1);
	_eeprom.settings.bat_max = MAX_CHG;
	_eeprom.settings.bat_min = MIN_CHG;
	
	_eeprom.scales_value.accuracy = 1;
	_eeprom.scales_value.average = 1;
	_eeprom.scales_value.filter = 100;
	_eeprom.scales_value.max = 1000;
	_eeprom.scales_value.step = 1;
	_eeprom.scales_value.zero_man_range = 0.02;
	_eeprom.scales_value.zero_on_range = 0.02;
	_eeprom.scales_value.zero_auto = 4;
	_eeprom.scales_value.enable_zero_auto = false;
	return _memory->save();
};

size_t BoardClass::doSettings(JsonObject& root) {
	JsonObject& scale = root.createNestedObject(SCALE_JSON);
	scale["bat_max"] = _eeprom.settings.bat_max;
	scale["bat_min"] = _eeprom.settings.bat_min;
	scale["id_assid"] = _eeprom.settings.apSSID;
	scale["id_nadmin"] = _eeprom.settings.user;
	scale["id_padmin"] = _eeprom.settings.password;	
	return root.measureLength();
};



void BoardClass::handleBinfo(AsyncWebServerRequest *request) {
	if (!request->authenticate(_eeprom.scales_value.user, _eeprom.scales_value.password))
		if (!server->checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	if (request->args() > 0) {
		bool flag = false;
		if (request->hasArg("bmax")) {
			float t = request->arg("bmax").toFloat();
			_eeprom.settings.bat_max = CONVERT_V_TO_ADC(t);
			flag = true;
		}
		if (flag) {
			if (request->hasArg("bmin")) {
				float t = request->arg("bmin").toFloat();
				_eeprom.settings.bat_min = CONVERT_V_TO_ADC(t);
			}
			else {
				flag = false;
			}				
		}
		if (flag && _memory->save()) {
			goto url;
		}
		return request->send(400);
	}
url:
	request->send(SPIFFS, request->url());
}

void BoardClass::parceCmd(JsonObject& cmd) {
	const char *command = cmd["cmd"];
	String strCmd = "";
	if (strcmp(command, "tp") == 0)
	{
#if !defined(DEBUG_WEIGHT_RANDOM)  && !defined(DEBUG_WEIGHT_MILLIS)
		if (Board->scales()->zero(_eeprom.scales_value.zero_man_range))
			SlaveScales.doTape();
#endif	
	}
	else if (strcmp(command, "sta") == 0)
	{
		_softConnect = cmd["con"];
		_softIp = cmd["ip"].as<String>();
		_softSSID = cmd["ssid"].as<String>();
		if (_softConnect)
			onSTA();
		else
			offSTA();
		cmd.printTo(strCmd);
		webSocket.textAll(strCmd);
		return;
	}
	else if (strcmp(command, "wt") == 0)
	{
		cmd.remove("cmd");
		weightHttpCmd(cmd);
	}
	else 
#ifdef MULTI_POINTS_CONNECT
	if (strcmp(command, "gpoint") == 0)
	{
		cmd.printTo(strCmd);
		webSocket.textAll(strCmd);
		return;		
	}
	else if (strcmp(command, "delpoint") == 0)
	{
		cmd.printTo(strCmd);
		webSocket.textAll(strCmd);
		return;		
	}
	else if (strcmp(command, "point") == 0)
	{
		cmd.printTo(strCmd);
		webSocket.textAll(strCmd);
		return;		
	}
	else
#else
	if (strcmp(command, "gnet") == 0) {
		cmd.printTo(strCmd);
		webSocket.textAll(strCmd);
		return;
	}else if (strcmp(command, "snet") == 0) {
		cmd.printTo(strCmd);
		webSocket.textAll(strCmd);
		return;
	}else	
#endif		 
	if (strcmp(command, "pause") == 0) {
		scales()->serialPause = true;
		return;
	}else if (strcmp(command, "resume") == 0) {
		scales()->serialPause = false;
		return;
	}else{
		return;
	}
	cmd.printTo(strCmd);
	Serial.println(strCmd);
};

void BoardClass::handleSeal(AsyncWebServerRequest * request) {
	randomSeed(_scales->readAverage());
	_eeprom.scales_value.seal = random(1000);
	
	if (_memory->save()) {
		return request->send(200, F("text/html"), String(_eeprom.scales_value.seal));
	}
	return request->send(400, F("text/html"), F("Ошибка!"));
}

bool BoardClass::saveEvent(const String& event, float value) {
	//String date = getDateTime();
	String str = "";
	DynamicJsonBuffer jsonBuffer;
	JsonObject &root = jsonBuffer.createObject();
	root["cmd"] = "swt";
	root["d"] = "";
	root["v"] = value;
	root["a"] = _scales->accuracy();
	
	root.printTo(str);
	Serial.println(str);
	webSocket.textAll(str);
	//Serial.flush();		
	return true;	
}

/** IP to String? */
String toStringIp(IPAddress ip) {
	String res = "";
	for (int i = 0; i < 3; i++) {
		res += String((ip >> (8 * i)) & 0xFF) + ".";
	}
	res += String(((ip >> 8 * 3)) & 0xFF);
	return res;
}