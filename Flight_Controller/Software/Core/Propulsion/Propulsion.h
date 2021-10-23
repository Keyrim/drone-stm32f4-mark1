/*
 * Propulsion.h
 *
 *  Created on: Dec 14, 2020
 *      Author: Théo
 */

#ifndef PROPULSION_PROPULSION_H_
#define PROPULSION_PROPULSION_H_


#include "../../Drivers/Inc/Motors.h"



typedef enum motors_names_e{
	MOTOR_BR = 0x00,
	MOTOR_BL,
	MOTOR_FR,
	MOTOR_FL,
	MOTOR_COUNT
}motors_names_e;

typedef enum Prop_consigne_axes_e{
	PROP_CONSIGNE_ROLL = 0x00,
	PROP_CONSIGNE_PITCH,
	PROP_CONSIGNE_YAW,
	PROP_CONSIGNE_THRUST,
	PROP_CONSIGNE_AXES_COUNT
}Prop_consigne_axes_e;

typedef struct propulsion_t{
	motors_t motors ;
	int16_t consigne[PROP_CONSIGNE_AXES_COUNT];
	uint16_t motors_outputs[MOTOR_COUNT];	//Value between 0 and 1000
}propulsion_t;

void PROPULSION_Init(propulsion_t * prop_, TIM_HandleTypeDef * htim);
void PROPULSION_Update_Motors(void);

#endif /* PROPULSION_PROPULSION_H_ */
