/*
 * Motors.c
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */


#include "Motors.h"

#define TIMER_MOTOR timer_e3
#define MOTOR_SIGNAL_MAX	500		/* Over 1000 */

static motor_t motor = { 0 };
static bool_e flag_start = TRUE;
static bool_e flag_stop = FALSE;



void MOTOR_Init(void)
{
	 TIMER_Start_All_Channels(TIMER_MOTOR);
}

void MOTOR_Process(void)
{
	if(flag_stop)
	{
		/* Clear flag */
		flag_stop = FALSE;
		/* Stop pwm on every channel */
		TIMER_Stop_All_Channels(TIMER_MOTOR);
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
		/* Start the pwm */
		TIMER_Start_All_Channels(TIMER_MOTOR);
		motor.is_enabled = TRUE;
	}
	if(motor.is_enabled)
	{
		TIMER_Set_All_CCR(TIMER_MOTOR, motor.output);
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
	target[0] = (uint16_t)MAX(target[0], 0);
	target[1] = (uint16_t)MAX(target[1], 0);
	target[2] = (uint16_t)MAX(target[2], 0);
	target[3] = (uint16_t)MAX(target[3], 0);

	target[0] = (uint16_t)MIN(target[0], MOTOR_SIGNAL_MAX);
	target[1] = (uint16_t)MIN(target[1], MOTOR_SIGNAL_MAX);
	target[2] = (uint16_t)MIN(target[2], MOTOR_SIGNAL_MAX);
	target[3] = (uint16_t)MIN(target[3], MOTOR_SIGNAL_MAX);

	motor.output[0] = (uint16_t)target[0]+1000;
	motor.output[1] = (uint16_t)target[1]+1000;
	motor.output[2] = (uint16_t)target[2]+1000;
	motor.output[3] = (uint16_t)target[3]+1000;
}
