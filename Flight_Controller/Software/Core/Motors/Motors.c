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
		.output_max = 700,
		.timer = timer_e3,
		.use_min_max_check = TRUE
};

static motor_t motor = { 0 };
static bool_e flag_start = TRUE;
static bool_e flag_stop = FALSE;



void MOTOR_Init(void)
{
	/* Load configuration */
	motor.config = default_motor_config;
	/* Start the timer */
	TIMER_Start_All_Channels(motor.config.timer);
}

void MOTOR_Process_Main(void)
{
	if(flag_stop)
	{
		/* Clear flag */
		flag_stop = FALSE;
		/* Stop pwm on every channel */
		TIMER_Stop_All_Channels(motor.config.timer);
		motor.is_enabled = FALSE;
	}
	else if(flag_start)
	{
		/* Clear flag */
		flag_start = FALSE;
		/* Reset target to 0 */
		motor.output[0] = 1000;
		motor.output[1] = 1000;
		motor.output[2] = 1000;
		motor.output[3] = 1000;
		/* Set the output to zero and start the timer */
		TIMER_Set_All_CCR(motor.config.timer, motor.output);
		TIMER_Start_All_Channels(motor.config.timer);
		motor.is_enabled = TRUE;
	}
}


/*
 * @brief Request to start the motors
 */
void MOTOR_Enable(void)
{
	flag_start = TRUE;
}

/*
 * @brief Request to stop the motors
 */
void MOTOR_Disable(void)
{
	flag_stop = TRUE;
}

/*
 * @brief update motor target values
 * @param array of new float values
 */
void MOTOR_Set(float * target)
{
	if(motor.config.use_min_max_check)
	{
		target[0] = (uint16_t)MAX(target[0], 0);
		target[1] = (uint16_t)MAX(target[1], 0);
		target[2] = (uint16_t)MAX(target[2], 0);
		target[3] = (uint16_t)MAX(target[3], 0);

		target[0] = (uint16_t)MIN(target[0], motor.config.output_max);
		target[1] = (uint16_t)MIN(target[1], motor.config.output_max);
		target[2] = (uint16_t)MIN(target[2], motor.config.output_max);
		target[3] = (uint16_t)MIN(target[3], motor.config.output_max);
	}
	motor.output[0] = (uint16_t)target[0]+999;
	motor.output[1] = (uint16_t)target[1]+999;
	motor.output[2] = (uint16_t)target[2]+999;
	motor.output[3] = (uint16_t)target[3]+999;
	TIMER_Set_All_CCR(motor.config.timer, motor.output);
}
