/*
 * High_Level.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Controller/Controller.h"
#include "../Radio/Radio.h"
#include "../Sensors/Mpu.h"

#define RADIO_TO_ANGLE			((float)0.1f)
#define RADIO_TO_ANGLE_RATE 	((float)0.5f)
#define RADIO_TO_ANGLE_RATE_YAW	((float)0.7f)

static high_level_t high_level = { 0 };
static high_level_state_t states[high_level_eCOUNT] =
{
		[high_level_eIDLE] =
		{
				.entrance = IDLE_Init,
				.main = IDLE_Main,
				.controller_state = controller_state_eDISABLED,
				.motor_state = motor_state_eENABLED,
				.orientation_state = orien_mode_eREAL
		},
		[high_level_eIDLE_NO_GYRO] =
		{
				.main = IDLE_NO_GYRO_Main,
				.controller_state = controller_state_eDISABLED,
				.motor_state = motor_state_eDISABLED,
				.orientation_state = orien_mode_eSIMULATION
		},
		[high_level_eSIMU] =
		{
				.main = SIMULATION_Main,
				.controller_state = controller_state_eDISABLED,
				.motor_state = motor_state_eSIMULATION,
				.orientation_state = orien_mode_eSIMULATION
		},
		[high_level_eSIMU_OPEN_LOOP] =
		{
				.main = SIMULATION_OPEN_LOOP_Main,
				.controller_state = controller_state_eDISABLED,
				.motor_state = motor_state_eENABLED,
				.orientation_state = orien_mode_eREAL
		}
};

void HIGH_LEVEL_Init(void)
{
	high_level.radio = RADIO_Get_Channel();
	high_level.target_angle = CONTROLLER_Get_Angle_Target();
	high_level.target_angle_speed = CONTROLLER_Get_Angle_Speed_Target();
	high_level.state = high_level_eSIMU_OPEN_LOOP;
	high_level.previous_state = high_level_eSIMU_OPEN_LOOP;

	/* Entrance and module config according to the default state */
	__disable_irq();
	CONTROLLER_Set_State(states[high_level.state].controller_state);
	MOTOR_Set_State(states[high_level.state].motor_state);
	ORIENTATION_Set_Mode(states[high_level.state].orientation_state);
	__enable_irq();
	if(states[high_level.state].entrance != NULL)
	{
		states[high_level.state].entrance(&high_level);
	}
}

void HIGH_LEVEL_Process_Main(void)
{
	if(!high_level.flag_ms)
	{
		return;
	}
	high_level.flag_ms = FALSE;
	if(high_level.state != high_level.previous_state)
	{
		if(states[high_level.previous_state].on_leave != NULL)
		{
			states[high_level.previous_state].on_leave(&high_level);
		}
		__disable_irq();
		CONTROLLER_Set_State(states[high_level.state].controller_state);
		MOTOR_Set_State(states[high_level.state].motor_state);
		ORIENTATION_Set_Mode(states[high_level.state].orientation_state);
		__enable_irq();
		if(states[high_level.state].entrance != NULL)
		{
			states[high_level.state].entrance(&high_level);
		}
		high_level.previous_state = high_level.state;
	}
	if(states[high_level.state].main != NULL)
	{
		states[high_level.state].main(&high_level);
	}
}

/*
 * @brief rise the "flag_ms" once every ms
 */
void HIGH_LEVEL_Process_Ms(void)
{
	high_level.flag_ms = TRUE;
}

/*
 * @brief change high level control registers values
 * @param control Control register id
 * @param value The new value
 */
void HIGH_LEVEL_Set_Control(high_level_control_e control, int16_t value)
{
	high_level.control[control] = value;
}











