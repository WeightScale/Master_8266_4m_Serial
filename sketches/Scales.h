#pragma once
#include "Config.h"
#include "HX711.h"
#include "Task.h"
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>

//#define DEBUG_WEIGHT_RANDOM
//#define DEBUG_WEIGHT_MILLIS

#define DOUT_PIN		GPIO16
#define SCK_PIN			GPIO14
#define RATE_PIN		RATE

typedef struct {
	bool isSave;
	int stabNum;
	float value;
	unsigned long time;
}t_save_value;

class ScalesClass : public HX711, public Task {
private:
	t_scales_value * _value;
	float _weight;
	float _round; /* ��������� ��� ���������� */
	float _stable_step; /* ��� ��� ������������ */
	bool _stableWeight = true;
	t_save_value _saveWeight = {0};
	float _weightSlave;
	long _offset_local;
	float _weight_zero_auto; /* ��� ���������� �� ���� */
public:
	ScalesClass(byte, byte, t_scales_value * value);
	~ScalesClass() {};
	void begin();
	void takeWeight();
	void fetchWeight();	
	long readAverage();
	long getValue();
	float getUnits();
	float getWeight();
	//bool isSave(){return _saveWeight.isSave;};
	void offset(long offset = 0){_offset_local = offset; };
	long offset(){return _offset_local;};
	int accuracy(){return _value->accuracy;};
	void average(unsigned char a) {_value->average = constrain(a, 1, 5);}
	unsigned char average(){return _value->average;};
	float weight(){return _weight;};
	bool overload() {return _weight > _value->max;};
	unsigned int seal(){ return _value->seal;};
	void mathRound() {
		_round = pow(10.0, _value->accuracy) / _value->step;  // ��������� ��� ����������}
		_stable_step = 1 / _round;
		//_weight_zero_auto = round(_value->zero_auto * _round) / _round;
		_weight_zero_auto = (float)_value->zero_auto / _round;
	}
	bool zero(float range);
	void detectStable(float);
	void detectAutoZero();
	size_t doData(JsonObject& json);	
	void formatValue(float value, char* string);
};