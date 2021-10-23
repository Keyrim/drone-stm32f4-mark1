/*
 * Propulsion.c
 *
 *  Created on: Dec 14, 2020
 *      Author: Théo
 */


#include "Propulsion.h"

static propulsion_t * prop ;

#define ROLL prop->consigne[PROP_CONSIGNE_ROLL]
#define PITCH prop->consigne[PROP_CONSIGNE_PITCH]
#define YAW prop->consigne[PROP_CONSIGNE_YAW]
#define THRUST prop->consigne[PROP_CONSIGNE_THRUST]

void PROPULSION_Init(propulsion_t * prop_, TIM_HandleTypeDef * htim){
	prop = prop_;

	MOTORS_Init(&prop->motors, htim, prop->motors_outputs, TRUE);
}

void PROPULSION_Update_Motors(void){

	//Mixage des sorties
	int16_t m1 = THRUST + PITCH - ROLL + YAW;
	int16_t m2 = THRUST + PITCH + ROLL - YAW ;
	int16_t m3 = THRUST - PITCH + ROLL + YAW ;
	int16_t m4 = THRUST - PITCH - ROLL - YAW ;
	prop->motors_outputs[MOTOR_FL] = (uint16_t)((m1 > 0) ? m1 : 0);
	prop->motors_outputs[MOTOR_FR] = (uint16_t)((m2 > 0) ? m2 : 0);
	prop->motors_outputs[MOTOR_BR] = (uint16_t)((m3 > 0) ? m3 : 0);
	prop->motors_outputs[MOTOR_BL] = (uint16_t)((m4 > 0) ? m4 : 0);
	MOTORS_Change_output(&prop->motors);

}
