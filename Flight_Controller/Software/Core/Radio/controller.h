/*
 * controller.h
 *
 *  Created on: Dec 12, 2020
 *      Author: Théo
 */

#ifndef RADIO_CONTROLLER_H_
#define RADIO_CONTROLLER_H_

#include "../../Drivers/Inc/Ibus.h"

#define NB_CHANNEL_MAX 	14
#define NB_CHANNEL 		10

typedef enum analysis_mode_e{
	ANALYSIS_MODE_NONE,
	ANALYSIS_MODE_STICK_DEAD_BAND,
	ANALYSIS_MODE_STICK_LVL,
	ANALYSIS_MODE_SEQUENCE,
	ANALYSIS_MODE_SWITCH_MODE,
	ANALYSIS_MODE_BUTTON_ON_OFF,
	ANALYSIS_MODE_BUTTON_PUSH
}analysis_mode_e;

typedef enum throttle_lvl_e{
	THROTTLE_LVL_NULL,
	THROTTLE_LVL_LOW,
	THROTTLE_LVL_HIGH
}throttle_lvl_e;

typedef enum switch_pos_e{
	SWITCH_POS_1,
	SWITCH_POS_2,
	SWITCH_POS_3,
	SWITCH_POS_ERROR
}switch_pos_e;

//Etat actuel du bouton
typedef enum button_state_e{
	BUTTON_STATE_NOT_PRESSED,
	BUTTON_STATE_PRESSED
}button_state_e;

//Fonctionne comme un interupteur => une pression change l'état
typedef enum button_on_off_e{
	BUTTON_ON_OFF_NOT_USED,
	BUTTON_OFF,
	BUTTON_ON
}button_on_off_e;

typedef enum button_pushed_e{
	BUTTON_PUSHED_NOT_USED,
	BUTTON_PUSHED_NO_REQUEST,
	BUTTON_PUSHED_REQUEST
}button_pushed_e;

typedef struct controller_t{

	//Ptr sur le tableaux contenant les valeurs des channels
	uint16_t * channels ;

	//Donne le mode d'analyse pour une channel donné
	analysis_mode_e analysis_mode [NB_CHANNEL_MAX];

	//Etat de la chanelle dans le mode qui lui est attribuée
	switch_pos_e switch_pos [NB_CHANNEL_MAX] ; //For buttons etc (0 = 1000, 1 = 1500, 2= 2000)
	throttle_lvl_e throttle_lvl [NB_CHANNEL_MAX] ;
	button_pushed_e button_pushed [NB_CHANNEL_MAX];
	button_on_off_e button_on_off [NB_CHANNEL_MAX];
	button_state_e button_state [NB_CHANNEL_MAX];
}controller_t;

void CONTROLLER_Init(controller_t * controller_, ibus_t * ibus);
bool_e CONTROLLER_Update_Channels_Array(void);
void CONTROLLER_Update_Channels_Analysis(void);
void CONTROLLER_Rx_Data_Rdy(void);

#endif /* RADIO_CONTROLLER_H_ */
