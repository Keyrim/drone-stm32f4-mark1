/*
 * events.h
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */


#ifndef EVENTS_EVENTS_H_
#define EVENTS_EVENTS_H_

#include "stm32f4xx_hal.h"
#include "mask_def_enum.h"
#include "mask.h"
#include "../system_d.h"


#define EVENT_NB_MASK_PER_EVENT_MAX 7

/*
 * @brief Events enumeration definition
 */
typedef enum
{
	//	----------------- Low lvl events (called before any task execution)----------------------
	EVENT_LOW_LVL_COUNT,


	//	----------------- High lvl events (called just  before the high lvl)----------------------
	//	Flights modes transition

	//Count
	EVENT_COUNT
}events_id_e;


typedef enum
{
	event_state_eDISABLED = 0,
	event_state_eENABLED
}event_state_e;

/*
 * @brief event structure definition
 */
typedef struct
{
	Mask_t mask_and[EVENT_NB_MASK_PER_EVENT_MAX] ;	/* "And" Conditions mask */
	Mask_t mask_or[EVENT_NB_MASK_PER_EVENT_MAX] ;	/* "Or" Conditions mask */
	Mask_t mask_not[EVENT_NB_MASK_PER_EVENT_MAX] ;  /* "Not" Conditions mask */
	uint32_t nb_mask ;
	event_state_e state ;
	void (*function)(mask_def_ids_e mask_id);
}Event_t;

void EVENT_init(void);
void EVENT_process(bool_e test_all);
void EVENT_Set_flag(flag_e flag);
void EVENT_Clear_flag(flag_e flag);
bool_e EVENT_Check_flag(flag_e flag);




#endif /* EVENTS_EVENTS_H_ */
