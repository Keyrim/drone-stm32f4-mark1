/*
 * Motor_Mixer.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#ifndef MOTOR_MIXER_H_
#define MOTOR_MIXER_H_

#include "../Task_Manager/macro_types.h"
#include "../Motors/Motors.h"

typedef struct
{
	/* Inputs */
	float input_angle[axe_eCOUNT];
	float input_power;
	/* Outputs */
	float output[motor_eCOUNT];
}motor_mixer_t;

void MOTOR_MIXER_Process(void);
float * MOTOR_MIXER_Get_Angle_Input_Ptr(void);
float * MOTOR_MIXER_Get_Power_Input_Ptr(void);

#endif /* MOTOR_MIXER_MOTOR_MIXER_H_ */
