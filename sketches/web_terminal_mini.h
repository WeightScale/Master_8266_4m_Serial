#pragma once

#define GPIO14			16							/* сигнал clock HX711 */
#define GPIO16			14							/* сигнал дата HX711 */
#define RATE			12							/* сигнал выборки АЦП  */
#define LED				2							/* индикатор работы */
#define V_BAT			A0							/* ацн для батареи */

#define BATTERY_6V				6
#define BATTERY_4V				4

#define R1_KOM					470.0f
#define R2_KOM					75.0f
#define VREF					1.0f
#define ADC						1023.0f

/*План питания от батареи*/
#define PLAN_BATTERY			BATTERY_6V
//#define PLAN_BATTERY			BATTERY_4V

#if PLAN_BATTERY == BATTERY_6V	
#define V_BAT_MAX				6.4f
#define V_BAT_MIN				5.5f				
#elif PLAN_BATTERY == BATTERY_4V
#define V_BAT_MAX				4.3f
#define V_BAT_MIN				3.3f
#endif // PLAN_BATTERY == BATTERY_6V