/*
 * Timer.c
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */


#include "Timer.h"


/*
 * @brief Timer structure definition
 */
typedef struct
{
	TIM_HandleTypeDef * htim;
}timer_t;

static timer_t timer[timer_eCOUNT] =
{
		[timer_e1] = (timer_t){&htim1},
		[timer_e3] = (timer_t){&htim3},
		[timer_e9] = (timer_t){&htim9},
		[timer_e10] = (timer_t){&htim10},
		[timer_e11] = (timer_t){&htim11}
};


void TIMER_Start(timer_e timer_id)
{
	HAL_TIM_Base_Start(timer[timer_id].htim);
}

void TIMER_Stop(timer_e timer_id)
{
	HAL_TIM_Base_Stop(timer[timer_id].htim);
}

uint32_t TIMER_Get_Tick(timer_e timer_id)
{
	return timer[timer_id].htim->Instance->CNT;
}

/*
 * @brief Start channels of a timer
 * @param timer_id The timer
 * @param channel The channel
 */
void TIMER_Start_Pwm(timer_e timer_id, uint16_t channel)
{
	HAL_TIM_PWM_Start(timer[timer_id].htim, channel);
}

/*
 * @brief Start all channels of a timer
 * @param timer_id id of the timer
 */
void TIMER_Start_Pwm_All_Channels(timer_e timer_id)
{
	HAL_TIM_PWM_Start(timer[timer_id].htim, TIM_CHANNEL_1);
	HAL_TIM_PWM_Start(timer[timer_id].htim, TIM_CHANNEL_2);
	HAL_TIM_PWM_Start(timer[timer_id].htim, TIM_CHANNEL_3);
	HAL_TIM_PWM_Start(timer[timer_id].htim, TIM_CHANNEL_4);
}

/*
 * @brief Stop channels of a timer
 * @param timer_id The timer
 * @param channel The channel
 */
void TIMER_Stop_Pwm(timer_e timer_id, uint16_t channel)
{
	HAL_TIM_PWM_Stop(timer[timer_id].htim, channel);
}

/*
 * @brief Stop all channels of a timer
 * @param timer_id id of the timer
 */
void TIMER_Stop_Pwm_All_Channels(timer_e timer_id)
{
	HAL_TIM_PWM_Stop(timer[timer_id].htim, TIM_CHANNEL_1);
	HAL_TIM_PWM_Stop(timer[timer_id].htim, TIM_CHANNEL_2);
	HAL_TIM_PWM_Stop(timer[timer_id].htim, TIM_CHANNEL_3);
	HAL_TIM_PWM_Stop(timer[timer_id].htim, TIM_CHANNEL_4);
}

/*
 * @brief Set the auto reload register value
 * @param timer_id Id of the timer
 * @param value New value
 */
void TIMER_Set_ARR(timer_e timer_id, uint16_t value)
{
	timer[timer_id].htim->Instance->ARR = value;
}

/*
 * @brief Set new capture compare values
 * @param tiemr_id The timer to use
 * @param values The new values
 */
void TIMER_Set_All_CCR(timer_e timer_id, uint16_t * values)
{
	timer[timer_id].htim->Instance->CCR1 = values[0];
	timer[timer_id].htim->Instance->CCR2 = values[1];
	timer[timer_id].htim->Instance->CCR3 = values[2];
	timer[timer_id].htim->Instance->CCR4 = values[3];
}










