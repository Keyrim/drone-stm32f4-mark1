/*
 * Manual_alti_auto.c
 *
 *  Created on: 7 févr. 2021
 *      Author: Théo
 */

#include "Flight_mode.h"

void manual_alti_auto_entrance(system_t * sys){
	REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_ANGLE);
	REGULATION_POSITION_Set_Regulation_Mode(REGULATION_POSITION_MODE_STABILIZED);
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_7, SEQUENCE_LED_OFF, SEQUENCE_LED_7);
	sys->regulation.position.consigne_position[POSITION_AXE_Z] = sys->position.position[POSITION_AXE_Z];
}
void manual_alti_auto_main(system_t * sys, uint32_t time_us){
	sys->regulation.position.consigne = sys->radio.controller.channels[2]-1000;
	sys->regulation.orientation.consigne_angular_pos[ORIENTATION_ROLL] = ((float)sys->radio.controller.channels[0]-1500)*0.14f;
	sys->regulation.orientation.consigne_angular_pos[ORIENTATION_PITCH] = ((float)sys->radio.controller.channels[1]-1500)*0.14f;
	sys->regulation.orientation.consigne_angular_speed[ORIENTATION_YAW] = -((float)sys->radio.controller.channels[3]-1500)*1.0f;
}
