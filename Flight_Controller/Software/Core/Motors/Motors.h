/*
 * Motors.h
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include "../Peripherals/Spi.h"

typedef struct
{

	float f_input;
	int16_t int_input;
}motor_t;

#endif /* MOTORS_H_ */
