		/*
 * Motors.c
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */


#include "Motors.h"
#include "math.h"

/*
 * @brief default motor configuration
 */
static const motor_config_t default_motor_config =
{
		.output_max = 500,
		.timer = timer_e3,
		.use_min_max_check = TRUE
};

static motor_t motor = { 0 };


/*
 * @brief Motor initialization process
 */
void MOTOR_Init(void)
{
	/* Load configuration */
	motor.config = default_motor_config;
	/* Start the timer */
	TIMER_Start_All_Channels(motor.config.timer);
}

/*
 * @brief Motor main ms process
 */
void MOTOR_Process_Ms(void)
{
	if(motor.state == motor_state_eENABLED)
	{
		return;
	}
	bool_e entrance = motor.state != motor.previous_state;
	switch(motor.state)
	{
		case motor_state_eDISABLED:
			TIMER_Stop_All_Channels(motor.config.timer);
			break;
		case motor_state_eSIMULATION:
			if(entrance)
			{
				TIMER_Stop_All_Channels(motor.config.timer);
			}
			break;
		case motor_state_eENABLED:
			/* We can not end here theoretically */
			break;
	}
	motor.previous_state = motor.state;
}

void MOTOR_Process_Gyro(void)
{
	if(motor.state != motor_state_eENABLED)
	{
		return;
	}
	bool_e entrance = motor.state != motor.previous_state;
	if(entrance)
	{
		motor.output[0] = 1000;
		motor.output[1] = 1000;
		motor.output[2] = 1000;
		motor.output[3] = 1000;
		TIMER_Set_All_CCR(motor.config.timer, motor.output);
		TIMER_Start_All_Channels(motor.config.timer);
	}
	TIMER_Set_All_CCR(motor.config.timer, motor.output);
	motor.previous_state = motor.state;
}

/*
 * @brief Change motor's module state
 * @param [in] new_state new motor's state
 */
void MOTOR_Set_State(motor_state_e new_state)
{
	motor.state = new_state;
}

/*
 * @brief update motor target values
 * @param array of new float values
 */
void MOTOR_Set(float * target)
{
	if(motor.state != motor_state_eDISABLED)
	{
		if(motor.config.use_min_max_check)
		{
			motor.output_float[0] = (uint16_t)MAX(target[0], 0);
			motor.output_float[1] = (uint16_t)MAX(target[1], 0);
			motor.output_float[2] = (uint16_t)MAX(target[2], 0);
			motor.output_float[3] = (uint16_t)MAX(target[3], 0);

			motor.output_float[0] = (uint16_t)MIN(motor.output_float[0], motor.config.output_max);
			motor.output_float[1] = (uint16_t)MIN(motor.output_float[1], motor.config.output_max);
			motor.output_float[2] = (uint16_t)MIN(motor.output_float[2], motor.config.output_max);
			motor.output_float[3] = (uint16_t)MIN(motor.output_float[3], motor.config.output_max);
		}
		else
		{
			motor.output_float[0] = target[0];
			motor.output_float[1] = target[1];
			motor.output_float[2] = target[2];
			motor.output_float[3] = target[3];
		}
		motor.output[motor_eFRONT_LEFT] = (uint16_t)motor.output_float[0]+999;
		motor.output[motor_eFRONT_RIGHT] = (uint16_t)motor.output_float[1]+999;
		motor.output[motor_eBACK_RIGHT] = (uint16_t)motor.output_float[2]+999;
		motor.output[motor_eBACK_LEFT] = (uint16_t)motor.output_float[3]+999;
	}
}

uint16_t * MOTOR_Get_Output(void)
{
	return motor.output;
}

float * MOTOR_Get_Output_Float(void)
{
	return motor.output_float;
}
