#include "Board.h"
#include "CalibratePage.h"
#include "SettingsPage.h"
#include "Battery.h"
#include "SlaveScales.h"
#include "BrowserServer.h"

CalibratePageClass * CalibratePage;
BrowserServerClass * server;

BoardClass::BoardClass() {
	_blink = new BlinkClass();
	_memory = new MemoryClass<MyEEPROMStruct>(&eeprom);
	if (!_memory->init()) {
		doDefault();
	}
	server = new BrowserServerClass(80, eeprom.settings.user, eeprom.settings.password);
	_battery = new BatteryClass(&eeprom.settings.bat_min, &eeprom.settings.bat_max);
	_wifi = new WiFiModuleClass(eeprom.settings.apSSID);
	_scales = new ScalesClass(DOUT_PIN, SCK_PIN, &eeprom.scales_value);
	//stationConnected = WiFi.onStationModeConnected(std::bind(&BoardClass::onStationModeConnected, this, std::placeholders::_1));	
	//stationDisconnected = WiFi.onStationModeDisconnected(std::bind(&BoardClass::onStationModeDisconnected, this, std::placeholders::_1));
	//_wifi->loadPoints();
	CalibratePage = new CalibratePageClass(&eeprom.scales_value);
	SettingsPage = new SettingsPageClass(&eeprom.settings);
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

bool BoardClass::doDefault() {
	String host = F("MASTER");
	String u = "admin";
	host.toCharArray(eeprom.settings.apSSID, host.length() + 1);
	u.toCharArray(eeprom.settings.user, u.length() + 1);
	u.toCharArray(eeprom.settings.password, u.length() + 1);
	u.toCharArray(eeprom.scales_value.user, u.length() + 1);
	u.toCharArray(eeprom.scales_value.password, u.length() + 1);
	eeprom.settings.bat_max = MAX_CHG;
	eeprom.settings.bat_min = MIN_CHG;
	
	eeprom.scales_value.accuracy = 1;
	eeprom.scales_value.average = 1;
	eeprom.scales_value.filter = 100;
	eeprom.scales_value.max = 1000;
	eeprom.scales_value.step = 1;
	eeprom.scales_value.zero_man_range = 0.02;
	eeprom.scales_value.zero_on_range = 0.02;
	return _memory->save();
};

size_t BoardClass::doSettings(JsonObject& root) {
	JsonObject& scale = root.createNestedObject(SCALE_JSON);
	scale["bat_max"] = eeprom.settings.bat_max;
	scale["bat_min"] = eeprom.settings.bat_min;
	scale["id_assid"] = eeprom.settings.apSSID;
	scale["id_n_admin"] = eeprom.settings.user;
	scale["id_p_admin"] = eeprom.settings.password;	
	return root.measureLength();
};

void BoardClass::handleBinfo(AsyncWebServerRequest *request) {
	if (!request->authenticate(eeprom.scales_value.user, eeprom.scales_value.password))
		if (!server->checkAdminAuth(request)) {
			return request->requestAuthentication();
		}
	if (request->args() > 0) {
		bool flag = false;
		if (request->hasArg("bmax")) {
			float t = request->arg("bmax").toFloat();
			eeprom.settings.bat_max = CONVERT_V_TO_ADC(t);
			flag = true;
		}
		if (flag) {
			if (request->hasArg("bmin")) {
				float t = request->arg("bmin").toFloat();
				eeprom.settings.bat_min = CONVERT_V_TO_ADC(t);
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
	
};

/** IP to String? */
String toStringIp(IPAddress ip) {
	String res = "";
	for (int i = 0; i < 3; i++) {
		res += String((ip >> (8 * i)) & 0xFF) + ".";
	}
	res += String(((ip >> 8 * 3)) & 0xFF);
	return res;
}