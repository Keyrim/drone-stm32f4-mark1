/*
 * buzzer.c
 *
 *  Created on: Feb 27, 2021
 *      Author: Théo
 */


#include "../Inc/buzzer.h"

uint16_t sequences[SEQUENCE_NOTES_COUNT][8] = {
		[SEQUENCE_NOTES_1]  = {DO4, 0, MI5, 0, SOL4, 0}
};

uint16_t sequences_len[SEQUENCE_NOTES_COUNT] = {
		[SEQUENCE_NOTES_1]  = 6
};

static void set_arr_from_frequency(buzzer_t * buzzer,  int32_t desired_frequency);
static void set_arr_from_frequency(buzzer_t * buzzer,  int32_t desired_frequency){
	if(!desired_frequency){
		buzzer->hal_state = HAL_TIM_PWM_Stop(buzzer->htim, TIM_CHANNEL_1);
	}
	else{
		int32_t tick_frequency = 180000000 / (buzzer->htim->Instance->PSC+1);
		int32_t value = (tick_frequency / desired_frequency) - 1 ;
		buzzer->htim->Instance->CCR1 = value / 5;
		buzzer->htim->Instance->ARR = value;
		buzzer->hal_state = HAL_TIM_PWM_Start(buzzer->htim, TIM_CHANNEL_1);
	}

}


void BUZZER_Init(buzzer_t * buzzer, TIM_HandleTypeDef * htim){
	buzzer->htim = htim;
	buzzer->current_seq = 0 ;
}

void BUZZER_Start(buzzer_t * buzzer, sequencies_e sequence){
	buzzer->current_seq = sequence ;
	buzzer->index = 0 ;
	buzzer->counter = 0;
}
void BUZZER_Stop(buzzer_t * buzzer){
	buzzer->hal_state = HAL_TIM_PWM_Stop(buzzer->htim, TIM_CHANNEL_1);
}

void BUZZER_Update(buzzer_t * buzzer){
	buzzer->counter++;
	buzzer->counter = buzzer->counter % 20 ;

	if(!buzzer->counter){
		set_arr_from_frequency(buzzer, sequences[buzzer->current_seq][buzzer->index++]);
		buzzer->index = buzzer->index % sequences_len[buzzer->current_seq];
	}
}
