/*
 * Controller.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#include "Controller.h"
#include "../Complementary_Filter/Complementary_Filter.h"
#include "../Sensors/Mpu.h"

static const controller_config_t default_controller_config =
{
		/* Pid Configuration 	 Roll 		Pitch 		Yaw */
		.angle_kp = 		{	4.0f, 		4.0f, 		0},
		.angle_speep_kp = 	{	0.8f, 		0.8f, 		1.2f}
};

static controller_t controller = { 0 };

void CONTROLLER_Init(void)
{
	/* Load configuration */
	controller.config = default_controller_config;
	controller.angle = COMPLEMENTARY_FILTER_Get_Angles();
	controller.angle_speed = MPU_Get_Gyro_Ptr();
	/* Initialize state to disabled by default */
	controller.state = controller_state_eDISABLED;
}

void CONTROLLER_Process_Gyro(void)
{
	/* We don t the "process gyro" if we are in simulation */
	if(controller.state == controller_state_eSIMULATION)
	{
		return;
	}
	/* Check for a new state */
	if(controller.new_state != controller.state)
	{
		controller.state = controller.new_state;
	}

	/* If controller is in angle mode, start with the angle pid */
	if(controller.state == controller_state_eANGLE)
	{
		/* Compute angle errors */
		controller.angle_error[axe_eROLL] = controller.target_angle[axe_eROLL] - controller.angle[axe_eROLL];
		controller.angle_error[axe_ePITCH] = controller.target_angle[axe_ePITCH] - controller.angle[axe_ePITCH];
		/* Handle angle continuity at -PI and + PI */
		//TODO ASAP angle error continuity
		/* Compute PID on angles */
		controller.target_angle_speed[axe_eROLL] = controller.config.angle_kp[axe_eROLL] * controller.angle_error[axe_eROLL];
		controller.target_angle_speed[axe_ePITCH] = controller.config.angle_kp[axe_ePITCH] * controller.angle_error[axe_ePITCH];
		/* Angular speed regulation */
		/* First thing first, errors */
		controller.angle_speed_error[axe_eROLL] = controller.target_angle_speed[axe_eROLL] - controller.angle_speed[axe_eROLL];
		controller.angle_speed_error[axe_ePITCH] = controller.target_angle_speed[axe_ePITCH] - controller.angle_speed[axe_ePITCH];
		controller.angle_speed_error[axe_eYAW] = controller.target_angle_speed[axe_eYAW] - controller.angle_speed[axe_eYAW];
		/* Now, PID's time ! */
		controller.output[axe_eROLL] = controller.angle_speed_error[axe_eROLL] * controller.config.angle_speep_kp[axe_eROLL];
		controller.output[axe_ePITCH] = controller.angle_speed_error[axe_ePITCH] * controller.config.angle_speep_kp[axe_ePITCH];
		controller.output[axe_eYAW] = controller.angle_speed_error[axe_eYAW] * controller.config.angle_speep_kp[axe_eYAW];
	}
	/* Angular speed regulation */
	if(controller.state == controller_state_eSPEED)
	{
		/* First thing first, errors */
		controller.angle_speed_error[axe_eROLL] = controller.target_angle_speed[axe_eROLL] - controller.angle_speed[axe_eROLL];
		controller.angle_speed_error[axe_ePITCH] = controller.target_angle_speed[axe_ePITCH] - controller.angle_speed[axe_ePITCH];
		controller.angle_speed_error[axe_eYAW] = controller.target_angle_speed[axe_eYAW] - controller.angle_speed[axe_eYAW];

		/* Now, PID's time ! */
		controller.output[axe_eROLL] = controller.angle_speed_error[axe_eROLL] * controller.config.angle_speep_kp[axe_eROLL];
		controller.output[axe_ePITCH] = controller.angle_speed_error[axe_ePITCH] * controller.config.angle_speep_kp[axe_ePITCH];
		controller.output[axe_eYAW] = controller.angle_speed_error[axe_eYAW] * controller.config.angle_speep_kp[axe_eYAW];
	}
}

void CONTROLLER_Process_ms(void)
{
	/* If we are not in simulation mode, we run in the "process gyro" */
	if(controller.state != controller_state_eSIMULATION)
	{
		return;
	}
	/* Does the state has to change ? */
	if(controller.new_state != controller.state)
	{
		controller.state = controller.new_state;
		return;
	}
	/* TODO Controller simulation mode => is it recquiered ? Can be done by the motors */
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











