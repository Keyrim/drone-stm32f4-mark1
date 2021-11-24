/*
 * Motors.h
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include "../Peripherals/Timer.h"
#include "../OS/macro_types.h"

typedef enum
{
	motor_eFRONT_LEFT = 0,
	motor_eFRONT_RIGHT,
	motor_eBACK_RIGHT,
	motor_eBACK_LEFT,
	motor_eCOUNT
}motor_e;

typedef struct
{
	bool_e is_enabled;
	uint16_t output[motor_eCOUNT];
}motor_t;

void MOTOR_Init(bool_e enable);
void MOTOR_Process(void);
void MOTOR_Enable(void);
void MOTOR_Disable(void);
#endif /* MOTORS_H_ */
