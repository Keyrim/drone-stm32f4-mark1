/*
 * Pid_settings.h
 *
 *  Created on: 24 janv. 2021
 *      Author: Théo
 */


#include "Flight_mode.h"

typedef enum pid_names_e{
	PID_ROLL_GYRO,
	PID_PITCH_GYRO,
	PID_YAW_GYRO,
	PID_ROLL,
	PID_PITCH
}pid_names_e;



void pid_settings_entrance(sytem_t * sys){
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_OFF, SEQUENCE_LED_4, SEQUENCE_LED_OFF);
}

void pid_settings_main(system_t * sys, uint32_t time_us){

}

