/*
 * Timer.c
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */


#include "Timer.h"

#define DEFINE_TIMER(htim_)		\
{								\
	.htim = htim_				\
}


/*
 * @brief Timer structure definition
 */
typedef struct
{
	TIM_HandleTypeDef * htim;
}timer_t;

static timer_t timer[timer_eCOUNT] =
{
		[timer_e3] = DEFINE_TIMER(&htim3)
};

/*
 * @brief Start channels of a timer
 * @param timer_id The timer
 * @param channel The channel
 */
void TIMER_Start(timer_e timer_id, uint16_t channel)
{
	HAL_TIM_PWM_Start(timer[timer_id].htim, channel);
}

/*
 * @brief Stop channels of a timer
 * @param timer_id The timer
 * @param channel The channel
 */
void TIMER_Stop(timer_e timer_id, uint16_t channel)
{
	HAL_TIM_PWM_Stop(timer[timer_id].htim, channel);
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










