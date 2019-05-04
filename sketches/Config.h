#pragma once
#include <Arduino.h>
#include "Version.h"

#define MASTER_USER				"sa"
#define MASTER_PASS				"343434"

#define WEB_TERMINAL2			1
#define WEB_TERMINAL_MINI		2
#define WEB_CRANE				3
#define SCALE_SERVER			4

//#define BOARD WEB_TERMINAL2
#define BOARD WEB_TERMINAL_MINI
//#define BOARD WEB_CRANE
//#define BOARD SCALE_SERVER

#if BOARD == WEB_TERMINAL2
	#include "web_terminal2.h"
#elif BOARD == WEB_TERMINAL_MINI
	#include "web_terminal_mini.h"
#elif BOARD == WEB_CRANE
	#include "web_crane.h"
#elif SCALE_SERVER
	#include "scale_server.h"
#endif // BOARD == WEB_TERMINAL2

#define HTML_PROGMEM          //Использовать веб страницы из flash памяти

#ifdef HTML_PROGMEM
	#include "Pages.h"
#endif

#define TIMEOUT_HTTP 3000			/** Время ожидания ответа HTTP запраса*/
#define STABLE_NUM_MAX 10			/** Количество стабильных измерений*/
#define MAX_EVENTS 100				/** Количество записей событий*/

typedef struct {
	char user[16];
	char password[16];
	char apSSID[33];
	char apKey[33];	
	unsigned int bat_max;
	unsigned int bat_min;
} settings_t;

typedef struct {
	bool rate;
	long offset; /*  */
	unsigned char average; /*  */
	unsigned char step; /*  */
	int accuracy; /*  */
	float max; /*  */
	float scale;
	unsigned char filter;
	unsigned int seal;
	float zero_man_range;    			// 0,2,4,10,20,50,100% диапазон обнуления
	float zero_on_range;     			// 0,2,4,10,20,50,100% диапазон обнуления
	unsigned char zero_auto;    			// 0~4 дискрет авто обнуление
	bool enable_zero_auto;					// использовать авто обнуление
	char user[16];
	char password[16];
}t_scales_value;

struct MyEEPROMStruct {
	settings_t settings;
	t_scales_value scales_value;
};

#define SCALE_JSON			"scale"
#define SERVER_JSON			"server"