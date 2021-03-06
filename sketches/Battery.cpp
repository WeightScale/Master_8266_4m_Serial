#include "Battery.h"

BatteryClass::BatteryClass(unsigned int *min, unsigned int *max) : Task(20000) {
	/* 20 ��������� ����� ������� */
	onRun(std::bind(&BatteryClass::fetchCharge, this));
	_max = max;
	_min = min;	
	fetchCharge();
#ifdef DEBUG_BATTERY
	_isDischarged = false;
#endif // DEBUG_BATTERY
}

unsigned int BatteryClass::fetchCharge() {
	_charge = _get_adc(1);
	_charge = constrain(_charge, *_min, *_max);
	_charge = map(_charge, *_min, *_max, 0, 100);
	_isDischarged = _charge <= 5;
	return _charge;
}

unsigned int BatteryClass::_get_adc(byte times) {
	unsigned long sum = 0;
#ifdef DEBUG_BATTERY
	for (byte i = 0; i < times; i++) {
		sum += random(ADC);
	}	
#else
	for (byte i = 0; i < times; i++) {
		sum += analogRead(V_BAT);
	}
#endif // DEBUG_BATTERY	
	return times == 0 ? sum : sum / times;	
}

size_t BatteryClass::doInfo(JsonObject& json) {
	json["id_min"] = CALCULATE_VIN(*_min);
	json["id_max"] = CALCULATE_VIN(*_max);
	json["id_cvl"] = CALCULATE_VIN(_get_adc(1));
	return json.measureLength();	
}

size_t BatteryClass::doData(JsonObject& json) {
	json["c"] = _charge;
	return json.measureLength();
};
