/*
 * Gyro_Acc_Calibration.c
 *
 *  Created on: 20 déc. 2020
 *      Author: theom
 */


#include "Flight_mode.h"
#include "../OS/events/events.h"
#include "../OS/events/flags.h"

void gyro_acc_calibration_entrance(system_t * sys){
	IHM_Set_Sequences(LED_SENSORS, SEQUENCE_LED_OFF, SEQUENCE_LED_OFF, SEQUENCE_LED_3);
}
void gyro_acc_calibration_main(system_t * sys, uint32_t time_us){
	static bool_e calibration_gyro_is_over = FALSE ;
	static bool_e calibration_acc_is_over = FALSE ;

	if(!calibration_gyro_is_over)
		calibration_gyro_is_over = 1;//GYRO_calibrate(&sys->sensors.gyro, 1500);
	if(!calibration_acc_is_over)
		calibration_acc_is_over = ACC_calibrate(&sys->sensors.acc, 1500);

	if(calibration_gyro_is_over && calibration_acc_is_over){	//The high lvl runs at 500 HZ so 1500 by 500 => 3 secs of calibration
		EVENT_Set_flag(FLAG_GYRO_CALI_DONE);
		IHM_Set_Sequences(LED_SENSORS, SEQUENCE_LED_OFF, SEQUENCE_LED_1, SEQUENCE_LED_OFF);
	}

}
