#pragma once
#include <Arduino.h>
#include <AsyncWebSocket.h>
#include <ArduinoJson.h>


#define TIMEOUT_SLAVE_CONNECT 7000		/* Время проверки в течении которого должны прийти данные от подчиненного для функции isConnect() */

class SlaveScalesClass : public AsyncWebSocket{
	protected:
		uint32 _clientId;
		AsyncWebSocketClient * _client;
		float _weight;
		int _accuracy;
		unsigned int _charge;
		bool _stableWeight;
		int _stableNum;
		bool _doTape;
		bool _connected = false;
		long _time_connect;
		String _url;
		String s_weight;
	public:
		SlaveScalesClass(const String& url);
		~SlaveScalesClass();		
		void events(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len);
		void init();
		void client(AsyncWebSocketClient * c){_client = c;};
		AsyncWebSocketClient * client(){return _client;};
		void doTape(){			
			if (_client) {
				_client->text("{\"tp\":}");
			}
		}		
		
		bool doValueUpdate(AsyncWebServerRequest * request);
		size_t doData(JsonObject& json);
		uint8_t charge(){return _charge;};
		
		float weight(){return _weight;};
		void weight(float w){_weight = w;};
		void accuracy(int a) {_accuracy = a;};
		int accuracy() {return _accuracy;};
	
		void stableWeight(bool s){_stableWeight = s;};
		bool stableWeight(){return _stableWeight;};
		
		void doTape(float t){_doTape = t;};
		bool isTape(){return _doTape;};
		bool isConnected(){
			if((_time_connect + TIMEOUT_SLAVE_CONNECT)> millis())
				return true;	
			return false;
		};
		String url(){return _url;};
};

extern SlaveScalesClass SlaveScales;