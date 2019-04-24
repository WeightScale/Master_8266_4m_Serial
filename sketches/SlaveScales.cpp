#include "SlaveScales.h"
#include <functional>
#include "BrowserServer.h"


using namespace std::placeholders;
SlaveScalesClass SlaveScales("/ss");

SlaveScalesClass::SlaveScalesClass(const String& url) : AsyncWebSocket(url){
	onEvent(std::bind(&SlaveScalesClass::events, this, _1, _2, _3, _4, _5, _6));
}

SlaveScalesClass::~SlaveScalesClass(){}
	
void SlaveScalesClass::init(){
	
	//onEvent(events);
}
	
void SlaveScalesClass::events(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){	
	if(type == WS_EVT_CONNECT){
		SlaveScales.client(client);
	} else if(type == WS_EVT_DISCONNECT){
		//_connected = false;
	} else if(type == WS_EVT_ERROR){
	} else if(type == WS_EVT_PONG){
	} else if(type == WS_EVT_DATA){
		_time_connect = millis();
		String msg = "";
		/*for(size_t i=0; i < len; i++) {
			msg += (char) data[i];
		}*/
		DynamicJsonBuffer jsonBuffer(len);
		JsonObject& root = jsonBuffer.parseObject(data);		
		if (!root.containsKey("cmd")) {
			return;
		}			
		const char *command = root["cmd"]; /* �������� ��������� �������*/
		JsonObject& json = jsonBuffer.createObject();
		json["cmd"] = command;
		if (strcmp(command, "swt") == 0){			// �������� ��������� ���� �������
			_weight = root["w"].as<float>();
			_accuracy = root["a"].as<int>();
			_charge = root["c"];
			_stableNum = root["s"].as<int>();	
		}else if (strcmp(command, "ip") == 0){
			_url = root["url"].as<String>();	
		}else if (strcmp(command, "dchg") == 0){
			webSocket.textAll("{\"cmd\":\"dchg\",\"rec\":\"SLAVE\"}");
		}else if (strcmp(command, "ovl") == 0){
			webSocket.textAll("{\"cmd\":\"ovl\",\"rec\":\"SLAVE\"}");
		}
		
		
		/*if (json.containsKey("slave"))
		{
			JsonObject& slave = json["slave"]; 
			_weight = slave["w"].as<float>();
			_accuracy = slave["a"].as<int>();
			_charge = slave["c"];
			_stableNum = slave["s"].as<int>();
		}
		else if (json.containsKey("ip"))
		{
			_url = json["ip"].as<String>();
		}
		else if (json.containsKey("dchg"))
		{
			ws.textAll("{\"cmd\":\"dchg\"}");
		}
		else if (json.containsKey("ovl"))
		{
			ws.textAll("{\"cmd\":\"dchg\"}");
		}*/
	}
}

bool SlaveScalesClass::doValueUpdate(AsyncWebServerRequest * request){	
	if (request->args() > 0) {
		if (request->hasArg("update")){
			DynamicJsonBuffer jsonBuffer;
			JsonObject& json = jsonBuffer.createObject();
			JsonObject& update = json.createNestedObject("up");
			update["ac"]= request->arg(F("weightAccuracy")).toInt();
			update["ws"]= request->arg(F("weightStep")).toInt();
			update["wa"]= request->arg(F("weightAverage")).toInt();
			update["wf"]= request->arg(F("weightFilter")).toInt();
			//update["wm"]= request->arg(F("weightMax")).toInt();
			size_t len = json.measureLength();
			AsyncWebSocketMessageBuffer * buffer = makeBuffer(len);
			if (buffer) {
				json.printTo((char *)buffer->get(), len + 1);
				if (_client) {
					_client->text(buffer);
					return true;
				}
			}			
		}		
	}
	return false;	
}

size_t SlaveScalesClass::doData(JsonObject& json) {
	json["w"] = _weight;
	json["a"] = _accuracy;
	json["s"] = _stableNum;
	json["c"] = _charge;
	return json.measureLength();	
};