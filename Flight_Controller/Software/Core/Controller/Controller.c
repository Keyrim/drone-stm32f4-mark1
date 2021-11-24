/*
 * Controller.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#include "Controller.h"
#include "../Complementary_Filter/Complementary_Filter.h"
#include "../Motor_Mixer/Motor_Mixer.h"

static controller_t controller =
{
		/* Pid Configuration 	 Roll 		Pitch 		Yaw*/
		.angle_kp = 			{10.0f, 	10.0f, 		0},
		.angle_speed_kp = 		{3.0f, 		3.0f, 		10.0f},
};

void CONTROLLER_Init(void)
{
	controller.angle = COMPLEMENTARY_FILTER_Get_Filter()->angle;
	controller.angle_speed = COMPLEMENTARY_FILTER_Get_Filter()->gyro;
	controller.output = MOTOR_MIXER_Get_Angle_Input_Ptr();

	controller.state = controller_state_eDISABLED;
}
void CONTROLLER_Process(void)
{
	if(controller.new_state != controller.state)
	{
		controller.state = controller.new_state;
	}
	if(controller.state)
	{
		/* A first PID to correct angle errors when we are in angle mode */
		if(controller.state == controller_state_eANGLE)
		{
			/* Compute angle errors */
			controller.angle_error[axe_eROLL] = controller.target_angle[axe_eROLL] - controller.angle[axe_eROLL];
			controller.angle_error[axe_ePITCH] = controller.target_angle[axe_ePITCH] - controller.angle[axe_ePITCH];
			/* Handle angle continuity at -PI and + PI */
			//TODO ASAP
			/* Compute PID on angles */
			controller.target_angle_speed[axe_eROLL] = controller.angle_kp[axe_eROLL] * controller.angle_error[axe_eROLL];
			controller.target_angle_speed[axe_ePITCH] = controller.angle_kp[axe_ePITCH] * controller.angle_error[axe_ePITCH];
		}
		/* Angular speed regulation */
		/* First thing first, errors */
		controller.angle_speed_error[axe_eROLL] = controller.target_angle_speed[axe_eROLL] - controller.angle_speed[axe_eROLL];
		controller.angle_speed_error[axe_ePITCH] = controller.target_angle_speed[axe_ePITCH] - controller.angle_speed[axe_ePITCH];
		controller.angle_speed_error[axe_eYAW] = controller.target_angle_speed[axe_eYAW] - controller.angle_speed[axe_eYAW];

		/* Now, PID's time ! */
		controller.output[axe_eROLL] = controller.angle_speed_error[axe_eROLL] * controller.angle_speed_kp[axe_eROLL];
		controller.output[axe_ePITCH] = controller.angle_speed_error[axe_ePITCH] * controller.angle_speed_kp[axe_ePITCH];
		controller.output[axe_eYAW] = controller.angle_speed_error[axe_eYAW] * controller.angle_speed_kp[axe_eYAW];
	}
	else
	{
		controller.output[axe_eROLL] = 0;
		controller.output[axe_ePITCH] = 0;
		controller.output[axe_eYAW] = 0;
	}
}

void CONTROLLER_Set_State(controller_state_e new_state)
{
	controller.new_state = new_state;
}

float * CONTROLLER_Get_Angle_Target(void)
{
	return controller.target_angle;
}

float * CONTROLLER_Get_Angle_Speed_Target(void)
{
	return controller.target_angle_speed;
}











