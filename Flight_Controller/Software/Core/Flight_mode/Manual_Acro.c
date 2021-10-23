/*
 * Manual_Acro.c
 *
 *  Created on: 19 déc. 2020
 *      Author: theom
 */


#include "Flight_mode.h"

void manual_accro_entrance(system_t * sys){
	REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_ACCRO);
	REGULATION_POSITION_Set_Regulation_Mode(REGULATION_POSITION_MODE_ON);
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_OFF, SEQUENCE_LED_OFF, SEQUENCE_LED_1);
}

void manual_accro_main(system_t * sys, uint32_t time_us){
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_ROLL] = ((float)sys->radio.controller.channels[0]-1500)*0.8f;
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_PITCH] = ((float)sys->radio.controller.channels[1]-1500)*0.8f;
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_YAW] = -((float)sys->radio.controller.channels[3]-1500)*1.2f;
	sys->regulation.position.consigne = sys->radio.controller.channels[2]-1000;
}
