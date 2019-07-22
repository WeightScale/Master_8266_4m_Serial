#pragma once
#include <FS.h>
#include <ESP_EEPROM.h>
#include "TaskController.h"
#include "Config.h"
#include "WiFiModule.h"
#include <ArduinoJson.h>
#include "Scales.h"
#include "Battery.h"
#include "SlaveScales.h"

template <typename T>class MemoryClass : protected EEPROMClass {
public:
	T * _value;

public:
	MemoryClass(T *mem) {
		_value = mem;
	};
	~MemoryClass() {
		close();
	};
	bool init() {
		SPIFFS.begin();	
		begin(sizeof(T));
		if (percentUsed() < 0)
			return false;
		get(0, *_value);
		return true;
	};
	bool save() {
		put(0, *_value);
		return commit();
	};
	void close() {
		end();
		SPIFFS.end();
	};
	bool doDefault();	
};
class BlinkClass : public Task {
public:
	unsigned int _flash = 500;
public:
	BlinkClass() : Task(500) {
		pinMode(LED, OUTPUT);
		ledOn();
		onRun(std::bind(&BlinkClass::blinkAP, this));
	};
	void blinkSTA() {
		static unsigned char clk;
		bool led = !digitalRead(LED);
		digitalWrite(LED, led);
		if (clk < 6) {
			led ? _flash = 70 : _flash = 40;
			clk++;
		}
		else {
			_flash = 2000;
			digitalWrite(LED, HIGH);
			clk = 0;
		}
		setInterval(_flash);
	}
	void blinkAP() {
		ledTogle();
		setInterval(500);
	}
	void ledOn() {digitalWrite(LED, LOW); };
	void ledOff() {digitalWrite(LED, HIGH); };
	void ledTogle() {digitalWrite(LED, !digitalRead(LED)); };
};

class BoardClass : public TaskController {
private:	
	struct MyEEPROMStruct _eeprom;
	BlinkClass *_blink;
	WiFiModuleClass * _wifi;
	//WiFiEventHandler stationConnected;
	//WiFiEventHandler stationDisconnected;	
	MemoryClass<MyEEPROMStruct> *_memory;
	ScalesClass *_scales;
	BatteryClass *_battery;
	bool _softConnect = true;				/* Флаг соединения softAP */
	String _softIp = "";
	String _softSSID = "";
	//BrowserServerClass *_server;
public :
	BoardClass();
	~BoardClass() {	
		delete _blink;
		delete _wifi;
		delete _memory;
		delete _scales;
		delete _battery;
	};
	void init() {
		_scales->begin();
		add(_blink);
		add(_wifi);
		add(_scales);
		add(_battery);
	};
	void handle() {
		run();
	};
	MemoryClass<MyEEPROMStruct> *memory() {return _memory;};
	WiFiModuleClass *wifi() {return _wifi;};
	ScalesClass *scales() {return _scales;};
	BatteryClass *battery() {return _battery;};
	bool doDefault();
	void onSTA() { _softConnect = true; _blink->onRun(std::bind(&BlinkClass::blinkSTA, _blink)); };
	void offSTA() { _softConnect = false; _blink->onRun(std::bind(&BlinkClass::blinkAP, _blink)); };
	//void onStationModeConnected(const WiFiEventStationModeConnected& evt);
	//void onStationModeDisconnected(const WiFiEventStationModeDisconnected& evt);	
	void parceCmd(JsonObject& cmd);
	String readSerial(uint32_t timeou = 5);
	size_t weightCmd(JsonObject& json);
	size_t weightHttpCmd(JsonObject& json);
	size_t doSettings(JsonObject& root);	
	void handleBinfo(AsyncWebServerRequest *request);
	void handleSeal(AsyncWebServerRequest * request);
	bool saveEvent(const String& event, float value);
	String softIp() {return _softIp;};
};

String toStringIp(IPAddress ip);
extern BoardClass * Board;