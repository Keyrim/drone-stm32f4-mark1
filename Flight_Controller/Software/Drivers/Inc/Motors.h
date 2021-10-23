/*
 * motors.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */

#ifndef INC_MOTORS_H_
#define INC_MOTORS_H_

#include "stm32f4xx_hal.h"
#include "../../Core/OS/macro_types.h"

typedef enum motors_state_e{
	MOTORS_STATE_IDDLE,
	MOTORS_STATE_RUNNING
}motors_state_e;

typedef struct motors_t{
	TIM_HandleTypeDef * htim;
	motors_state_e state ;
	uint16_t * duty;
	HAL_StatusTypeDef hal_state;
}motors_t;

void MOTORS_Init(motors_t * motors, TIM_HandleTypeDef * htim, uint16_t * duty, bool_e start);
void MOTORS_Start(motors_t * motors);
void MOTORS_Stop(motors_t * motors);
void MOTORS_Change_output(motors_t * motors);

#endif /* INC_MOTORS_H_ */
