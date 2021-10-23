/*
 * events.h
 *
 *  Created on: 13 sept. 2020
 *      Author: Th�o
 */


#ifndef EVENTS_EVENTS_H_
#define EVENTS_EVENTS_H_

#include "stm32f4xx_hal.h"
#include "mask_def_enum.h"
#include "mask.h"
#include "../system_d.h"


#define EVENT_NB_MASK_PER_EVENT_MAX 7

typedef enum events_id_main_e{
	//	----------------- Low lvl events (called before any task execution)----------------------
	EVENT_GYRO_DATA_READY = 0x00,
	EVENT_ACC_DATA_READY,
	EVENT_ORIENTATION_UPDATE,
	EVENT_IBUS_DATA_RDY,
	EVENT_GYRO_INIT_OK,
	EVENT_ACC_INIT_OK,
	EVENT_BARO_DATA_RDY,
	//Low lvl event's count
	EVENT_LOW_LVL_COUNT,


	//	----------------- High lvl events (called jsut before the high lvl)----------------------
	//	Flights modes transition
	EVENT_ON_THE_GROUND = EVENT_LOW_LVL_COUNT,
	EVENT_MANUAL_ACCRO,
	EVENT_MANUAL_ANGLE,
	EVENT_MANUAL_ANGLE_ALTITUDE_AUTO,
	EVENT_ARMING,
	EVENT_GYRO_ACC_CALIBRATION,

	//Count
	EVENT_COUNT
}events_id_main_e;		//Events déclenchés en tâche de fond


typedef enum{
	EVENT_TYPE_HIGH_LVL,
	EVENT_TYPE_ERROR
}Event_type_t;

typedef enum Event_state_e{
	EVENT_ENABLED,
	EVENT_DISABLED
}Event_state_e;

typedef struct{
	Mask_t mask_and[EVENT_NB_MASK_PER_EVENT_MAX] ;	//Masques de conditions et
	Mask_t mask_or[EVENT_NB_MASK_PER_EVENT_MAX] ;	//Masques de déclenchement
	Mask_t mask_not[EVENT_NB_MASK_PER_EVENT_MAX] ; 	//Masques de conditions non
	uint32_t nb_mask ;								//Nombre de paires de masque
	Event_state_e state ;
	void (*function)(mask_def_ids_t mask_id);
}Event_t;


//Init event
void EVENT_init(system_t * sys_);

//Process events handler
void EVENT_process(bool_e test_all);


//Set and clean flags
void EVENT_Set_flag(Flags_e flag);
void EVENT_Clean_flag(Flags_e flag);
bool_e EVENT_Check_flag(Flags_e flag);




#endif /* EVENTS_EVENTS_H_ */
