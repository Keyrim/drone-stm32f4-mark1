/*
 * Motor_mixer.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#include "Motor_Mixer.h"

static motor_mixer_t motor_mixer = { 0 };

void MOTOR_MIXER_Process(void)
{
	motor_mixer.output[motor_eFRONT_LEFT] = 	motor_mixer.input_power + motor_mixer.input_angle[axe_eROLL] - motor_mixer.input_angle[axe_ePITCH] - motor_mixer.input_angle[axe_eYAW];
	motor_mixer.output[motor_eFRONT_RIGHT] = 	motor_mixer.input_power - motor_mixer.input_angle[axe_eROLL] - motor_mixer.input_angle[axe_ePITCH] + motor_mixer.input_angle[axe_eYAW];
	motor_mixer.output[motor_eBACK_RIGHT] = 	motor_mixer.input_power - motor_mixer.input_angle[axe_eROLL] + motor_mixer.input_angle[axe_ePITCH] - motor_mixer.input_angle[axe_eYAW];
	motor_mixer.output[motor_eBACK_LEFT] = 		motor_mixer.input_power + motor_mixer.input_angle[axe_eROLL] + motor_mixer.input_angle[axe_ePITCH] + motor_mixer.input_angle[axe_eYAW];

	MOTOR_Set(motor_mixer.output);
}
float * MOTOR_MIXER_Get_Angle_Input_Ptr(void)
{
	return motor_mixer.input_angle;
}
float * MOTOR_MIXER_Get_Power_Input_Ptr(void)
{
	return &motor_mixer.input_power;
}
