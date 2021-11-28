/*
 * Motors.h
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include "../Peripherals/Timer.h"
#include "../Task_Manager/macro_types.h"

typedef enum
{
	motor_eFRONT_LEFT 	= 3,
	motor_eFRONT_RIGHT	= 1,
	motor_eBACK_RIGHT	= 0,
	motor_eBACK_LEFT	= 2,
	motor_eCOUNT		= 4
}motor_e;

typedef struct
{
	bool_e is_enabled;
	uint16_t output[motor_eCOUNT];
}motor_t;

void MOTOR_Init(void);
void MOTOR_Process(void);
void MOTOR_Enable(void);
void MOTOR_Disable(void);
void MOTOR_Set(float * target);
#endif /* MOTORS_H_ */
