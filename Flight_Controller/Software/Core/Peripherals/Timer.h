/*
 * Timer.h
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "tim.h"

/*
 * @brief Timer enumeration definition
 */
typedef enum
{
	timer_e3,
	timer_eCOUNT,
	/* Rename for an easier use */
	timer_eMOTORS = timer_e3
}timer_e;



void TIMER_Start(timer_e timer_id, uint16_t channel);
void TIMER_Stop(timer_e timer_id, uint16_t channel);
void TIMER_Set_ARR(timer_e timer_id, uint16_t value);
void TIMER_Set_All_CCR(timer_e timer_id, uint16_t * values);

#endif /* TIMER_H_ */
