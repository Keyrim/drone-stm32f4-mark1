/*
 * Controller.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#include "Controller.h"
static controller_t controller = { 0 };

void CONTROLLER_Init(float * angle, float * angle_speed)
{
	controller.angle = angle;
	controller.angle_speed = angle_speed;

	controller.state = controller_state_eDISABLED;
}
void CONTROLLER_Process(void)
{
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

