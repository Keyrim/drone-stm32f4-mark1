/*
 * On_the_ground.c
 *
 *  Created on: 19 déc. 2020
 *      Author: theom
 */
#include "Flight_mode.h"

void on_the_ground_entrance(system_t * sys){
	REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_OFF);
	REGULATION_POSITION_Set_Regulation_Mode(REGULATION_POSITION_MODE_OFF);
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_OFF, SEQUENCE_LED_1, SEQUENCE_LED_OFF);
}
void on_the_ground_main(system_t * sys, uint32_t time_us){
}
