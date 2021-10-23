/*
 * arming.c
 *
 *  Created on: 19 déc. 2020
 *      Author: theom
 */


#include "Flight_mode.h"
#include "../OS/events/flags.h"
#include "../OS/events/events.h"


typedef enum state_arming_e{
	ARMING_THROTTLE_LOW = 0,
	ARMING_STICK_ON_THE_RIGHT,
	ARMING_VROUM_VOUM
}state_arming_e;

#define MOTORS_HIGH_POWER 	200
#define MOTORS_LOW_POWER	40

#define PERIODE_1 	400000
#define PERIODE_2 	300000
#define PERIODE_3 	200000
#define PERIODE_4 	500000


static state_arming_e state = 0;
static uint8_t state_vroum_vroum = 0;
static uint32_t next_change = 0 ;
static float elapsed = 0 ;

void arming_entrance(system_t * sys){
	REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_OFF);
	REGULATION_POSITION_Set_Regulation_Mode(REGULATION_POSITION_MODE_ON);
	sys->regulation.position.consigne = MOTORS_LOW_POWER;
	IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_10, SEQUENCE_LED_OFF, SEQUENCE_LED_OFF);
	state = ARMING_THROTTLE_LOW ;
	state_vroum_vroum= 0 ;
}

void arming_main(system_t * sys, uint32_t time_us){



	switch(state){
		case ARMING_THROTTLE_LOW :
			if(sys->radio.controller.channels[2]<1100){
				IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_11, SEQUENCE_LED_OFF, SEQUENCE_LED_OFF);
				state = ARMING_STICK_ON_THE_RIGHT;
			}
			break;
		case ARMING_STICK_ON_THE_RIGHT:
			if(sys->radio.controller.channels[3]>1950 && sys->radio.controller.channels[2]<1100){
				IHM_Set_Sequences(LED_HIGH_LVL, SEQUENCE_LED_ON, SEQUENCE_LED_OFF, SEQUENCE_LED_OFF);
				state = ARMING_VROUM_VOUM;
			}
			break;
		case ARMING_VROUM_VOUM:
			switch(state_vroum_vroum){
				case 0:
					next_change = time_us + PERIODE_1;
					sys->regulation.position.consigne = MOTORS_HIGH_POWER;
					state_vroum_vroum++;
					break;
				case 1:
					elapsed = (float)time_us - (float)(next_change-PERIODE_1);
					elapsed /= (float)PERIODE_1;
					sys->regulation.position.consigne = MOTORS_LOW_POWER + (uint32_t)((float)(MOTORS_HIGH_POWER - MOTORS_LOW_POWER) * elapsed);
					if(time_us > next_change){
						next_change = time_us + PERIODE_2;
						sys->regulation.position.consigne = MOTORS_LOW_POWER;
						state_vroum_vroum++;
					}
					break;
				case 2:
					if(time_us > next_change){
						next_change = time_us + PERIODE_3;
						sys->regulation.position.consigne = MOTORS_HIGH_POWER;
						state_vroum_vroum++;
					}
					break;
				case 3:
					if(time_us > next_change){
						next_change = time_us + PERIODE_4;
						sys->regulation.position.consigne = MOTORS_LOW_POWER;
						state_vroum_vroum++;
					}
					break;
				case 4:
					if(time_us > next_change){
						EVENT_Set_flag(FLAG_ARMED);
						state_vroum_vroum++;
					}
					break;
				default:
					break;
			}
			break;
	}
}







