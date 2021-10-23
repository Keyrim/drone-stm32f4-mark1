/*
 * Motors.c
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */

#include "../Inc/Motors.h"


void MOTORS_Init(motors_t * motors, TIM_HandleTypeDef * htim, uint16_t * duty, bool_e start){
	motors->htim = htim ;
	motors->duty = duty;
	if(start){
		motors->state = MOTORS_STATE_RUNNING ;
		motors->hal_state = HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
		motors->hal_state = HAL_TIM_PWM_Start(htim, TIM_CHANNEL_2);
		motors->hal_state = HAL_TIM_PWM_Start(htim, TIM_CHANNEL_3);
		motors->hal_state = HAL_TIM_PWM_Start(htim, TIM_CHANNEL_4);
	}
}
void MOTORS_Start(motors_t * motors){
	motors->state = MOTORS_STATE_RUNNING ;
	motors->hal_state = HAL_TIM_PWM_Start(motors->htim, TIM_CHANNEL_1);
	motors->hal_state = HAL_TIM_PWM_Start(motors->htim, TIM_CHANNEL_2);
	motors->hal_state = HAL_TIM_PWM_Start(motors->htim, TIM_CHANNEL_3);
	motors->hal_state = HAL_TIM_PWM_Start(motors->htim, TIM_CHANNEL_4);
}

void MOTORS_Stop(motors_t * motors){
	motors->state = MOTORS_STATE_IDDLE ;
	motors->hal_state = HAL_TIM_PWM_Stop(motors->htim, TIM_CHANNEL_1);
	motors->hal_state = HAL_TIM_PWM_Stop(motors->htim, TIM_CHANNEL_2);
	motors->hal_state = HAL_TIM_PWM_Stop(motors->htim, TIM_CHANNEL_3);
	motors->hal_state = HAL_TIM_PWM_Stop(motors->htim, TIM_CHANNEL_4);
}

void MOTORS_Change_output(motors_t * motors){

	motors->duty[0] = MIN(1000, motors->duty[0]);
	motors->duty[1] = MIN(1000, motors->duty[1]);
	motors->duty[2] = MIN(1000, motors->duty[2]);
	motors->duty[3] = MIN(1000, motors->duty[3]);


	motors->htim->Instance->CCR1 = motors->duty[0]+999;
	motors->htim->Instance->CCR2 = motors->duty[1]+999;
	motors->htim->Instance->CCR3 = motors->duty[2]+999;
	motors->htim->Instance->CCR4 = motors->duty[3]+999;
}
