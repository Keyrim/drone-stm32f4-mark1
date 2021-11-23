/*
 * Motors.h
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include "../Peripherals/Spi.h"

typedef enum
{
	motor_state_eLOCKED,
	motor_state_eUNLOCKED
}motor_state_e;

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
	motor_state_e state;
	float target[motor_eCOUNT];
	int16_t target_linearized[motor_eCOUNT];
}motor_t;

#endif /* MOTORS_H_ */
