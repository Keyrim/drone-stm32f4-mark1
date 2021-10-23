/*
 * controller.c
 *
 *  Created on: Dec 12, 2020
 *      Author: Théo
 */


#include "controller.h"
#include "../OS/events/events.h"
#include "stdlib.h"

#define JOYCTICK_DEAD_BAND 30

//Controller used
static controller_t * controller ;
//Private functions prototypes
static void flag_process();

void CONTROLLER_Init(controller_t * controller_, ibus_t * ibus){
	//Sauvegarde la structure utilsié
	controller = controller_ ;
	controller->channels = ibus->channels ;

	//Configuration du mode d'analyse pour les channels
	controller->analysis_mode[0] = ANALYSIS_MODE_STICK_DEAD_BAND ;
	controller->analysis_mode[1] = ANALYSIS_MODE_STICK_DEAD_BAND ;
	controller->analysis_mode[2] = ANALYSIS_MODE_STICK_LVL ;
	controller->analysis_mode[3] = ANALYSIS_MODE_STICK_DEAD_BAND ;
	controller->analysis_mode[4] = ANALYSIS_MODE_SWITCH_MODE ;
	controller->analysis_mode[5] = ANALYSIS_MODE_SWITCH_MODE ;
	controller->analysis_mode[6] = ANALYSIS_MODE_SWITCH_MODE ;
	controller->analysis_mode[7] = ANALYSIS_MODE_SWITCH_MODE ;
	controller->analysis_mode[8] = ANALYSIS_MODE_BUTTON_PUSH ;
	controller->analysis_mode[9] = ANALYSIS_MODE_BUTTON_ON_OFF ;

}

void CONTROLLER_Update_Channels_Analysis(void){
	//Analyse des channels
		bool_e button_state ;
		for(int8_t ch = 0; ch < NB_CHANNEL; ch++ ){
			switch (controller->analysis_mode[ch]) {
				case ANALYSIS_MODE_NONE:
					//On fait R
					break;

				case ANALYSIS_MODE_STICK_DEAD_BAND :
					if(abs(controller->channels[ch] - 1500) < JOYCTICK_DEAD_BAND)
						controller->channels[ch] = 1500 ;
					break;

				case ANALYSIS_MODE_STICK_LVL :
					if(controller->channels[ch] > 1000 && controller->channels[ch] <= 1050)
						controller->throttle_lvl[ch] = THROTTLE_LVL_NULL ;
					else if(controller->channels[ch] > 1050 && controller->channels[ch] <= 1150)
						controller->throttle_lvl[ch] = THROTTLE_LVL_LOW ;
					else if(controller->channels[ch] > 1150 && controller->channels[ch] < 2000)
						controller->throttle_lvl[ch] = THROTTLE_LVL_HIGH ;


					break;

				case ANALYSIS_MODE_BUTTON_ON_OFF :
					//Active le boutton dans le code
					if(!controller->button_on_off[ch])
						controller->button_on_off[ch] = BUTTON_OFF ;
					//Buton state
					button_state = controller->channels[ch] > 1500 ;
					//If different from previous state and previous was low
					if(button_state != controller->button_state[ch] && controller->button_state[ch] == 0){
						if(controller->button_on_off[ch] == BUTTON_ON)
							controller->button_on_off[ch] = BUTTON_OFF ;
						else
							controller->button_on_off[ch] = BUTTON_ON ;
					}

					controller->button_state[ch] = button_state ;
					break;

				case ANALYSIS_MODE_BUTTON_PUSH :
					//Active le boutton dans le code
					if(!controller->button_pushed[ch])
						controller->button_pushed[ch] = BUTTON_PUSHED_NO_REQUEST ;
					//Buton state
					button_state = controller->channels[ch] > 1500 ;
					//If different from previous state and previous was low
					if(button_state != controller->button_state[ch] && controller->button_state[ch] == 0){
						controller->button_pushed[ch] = BUTTON_PUSHED_REQUEST ;
					}

					controller->button_state[ch] = button_state ;
					break;

				case ANALYSIS_MODE_SEQUENCE:
					//todo analyse de séquences sur les switchs
					break;

				case ANALYSIS_MODE_SWITCH_MODE:
					if(controller->channels[ch] < 1300)
						controller->switch_pos[ch] = SWITCH_POS_1 ;
					else if(controller->channels[ch] > 1300 && controller->channels[ch] < 1600)
						controller->switch_pos[ch] = SWITCH_POS_2 ;
					else if(controller->channels[ch] > 1600)
						controller->switch_pos[ch] = SWITCH_POS_3 ;
					else
						controller->switch_pos[ch] = SWITCH_POS_ERROR ;
					break;
				default:
					break;
			}
		}

		flag_process();
}

static void flag_process()
{
	//Throttle lvl
	switch(controller->throttle_lvl[2]){
		case THROTTLE_LVL_NULL :
			EVENT_Set_flag(FLAG_THROTTLE_NULL);
			EVENT_Clean_flag(FLAG_THROTTLE_LOW);
			EVENT_Clean_flag(FLAG_THROTTLE_HIGH);
			break;
		case THROTTLE_LVL_LOW :
			EVENT_Clean_flag(FLAG_THROTTLE_NULL);
			EVENT_Set_flag(FLAG_THROTTLE_LOW);
			EVENT_Clean_flag(FLAG_THROTTLE_HIGH);
			break;
		case THROTTLE_LVL_HIGH :
			EVENT_Clean_flag(FLAG_THROTTLE_NULL);
			EVENT_Clean_flag(FLAG_THROTTLE_LOW);
			EVENT_Set_flag(FLAG_THROTTLE_HIGH);
			break;
	}

	//Switch 1 (ch5 soit ch[4])
	if(controller->switch_pos[4] == SWITCH_POS_1){
		EVENT_Set_flag(FLAG_CHAN_5_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_5_POS_3);
	}else{
		EVENT_Clean_flag(FLAG_CHAN_5_POS_1);
		EVENT_Set_flag(FLAG_CHAN_5_POS_3);
	}
	//Switch 2(ch6 soit ch[5])
	switch(controller->switch_pos[5]){
		case SWITCH_POS_1:
			EVENT_Set_flag(FLAG_CHAN_6_POS_1);
			EVENT_Clean_flag(FLAG_CHAN_6_POS_2);
			EVENT_Clean_flag(FLAG_CHAN_6_POS_3);
			break;
		case SWITCH_POS_2:
			EVENT_Clean_flag(FLAG_CHAN_6_POS_1);
			EVENT_Set_flag(FLAG_CHAN_6_POS_2);
			EVENT_Clean_flag(FLAG_CHAN_6_POS_3);
			break;
		case SWITCH_POS_3:
			EVENT_Clean_flag(FLAG_CHAN_6_POS_1);
			EVENT_Clean_flag(FLAG_CHAN_6_POS_2);
			EVENT_Set_flag(FLAG_CHAN_6_POS_3);
			break;
		case SWITCH_POS_ERROR:
			break;
	}
	//Switch 3(ch7 soit ch[6])
	switch(controller->switch_pos[6]){
		case SWITCH_POS_1:
			EVENT_Set_flag(FLAG_CHAN_7_POS_1);
			EVENT_Clean_flag(FLAG_CHAN_7_POS_2);
			EVENT_Clean_flag(FLAG_CHAN_7_POS_3);
			break;
		case SWITCH_POS_2:
			EVENT_Clean_flag(FLAG_CHAN_7_POS_1);
			EVENT_Set_flag(FLAG_CHAN_7_POS_2);
			EVENT_Clean_flag(FLAG_CHAN_7_POS_3);
			break;
		case SWITCH_POS_3:
			EVENT_Clean_flag(FLAG_CHAN_7_POS_1);
			EVENT_Clean_flag(FLAG_CHAN_7_POS_2);
			EVENT_Set_flag(FLAG_CHAN_7_POS_3);
			break;
		case SWITCH_POS_ERROR:
			break;
	}
	//Switch 4 (ch8 soit ch[7])
	if(controller->switch_pos[7] == SWITCH_POS_1){
		EVENT_Set_flag(FLAG_CHAN_8_POS_1);
		EVENT_Clean_flag(FLAG_CHAN_8_POS_3);
	}else{
		EVENT_Clean_flag(FLAG_CHAN_8_POS_1);
		EVENT_Set_flag(FLAG_CHAN_8_POS_3);
	}

	//Button 1 (ch9 soit ch[8]) en mode push
	if(controller->button_pushed[8] == BUTTON_PUSHED_REQUEST){
		EVENT_Set_flag(FLAG_CHAN_9_PUSH);
		controller->button_pushed[8] = BUTTON_PUSHED_NO_REQUEST ;
	}

	//Button 2 (ch10 soit ch[9]) en mode on/off
	if(controller->button_on_off[9] == BUTTON_ON)
		EVENT_Set_flag(FLAG_CHAN_10_ON);
	else
		EVENT_Clean_flag(FLAG_CHAN_10_ON);



}

bool_e CONTROLLER_Update_Channels_Array(void){
	//Utilisation de l'ibus atm
	return IBUS_Channel_Update();
}

void CONTROLLER_Rx_Data_Rdy(void){
	EVENT_Set_flag(FLAG_IBUS_DATA_RDY);
}
