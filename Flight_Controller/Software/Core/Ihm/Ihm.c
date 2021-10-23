/*
 * Ihm.c
 *
 *  Created on: 16 déc. 2020
 *      Author: Théo
 */

#include "Ihm.h"


#define SEQUENCE_LEN 12

static Ihm_t * ihm ;
static uint16_t pin_color[LED_COLOR_COUNT] = {GPIO_PIN_13, GPIO_PIN_14, GPIO_PIN_15};


static uint32_t compteur = 0 ;
static led_name_e current_led ;





void IHM_Init(Ihm_t * ihm_){
	ihm = ihm_;
}

void IHM_Led_Init(led_name_e name, uint32_t periode, uint16_t pin, led_output_logic_e output_logic){
	ihm->leds[name].periode = periode ;
	ihm->leds[name].pin = pin ;
	ihm->leds[name].output_logic = output_logic ;

	//Set the outputs low
	for(uint8_t c = 0 ; c < LED_COLOR_COUNT; c++)
		IHM_Set_Sequence(name, c, SEQUENCE_LED_OFF);
}

void IHM_Set_Sequence(led_name_e name, led_color_e color, uint16_t sequence){
	if(ihm->leds[name].output_logic == LED_OUTPUT_NEGATIVE)
		sequence = ~sequence ;
	ihm->leds[name].seqs[color] = sequence ;
}

void IHM_Set_Sequences(led_name_e name, uint16_t sequence_red, uint16_t sequence_green, uint16_t sequence_blue){
	if(ihm->leds[name].output_logic == LED_OUTPUT_NEGATIVE){
		sequence_red = ~sequence_red ;
		sequence_green = ~sequence_green ;
		sequence_blue = ~sequence_blue ;
	}
	ihm->leds[name].seqs[LED_COLOR_RED] = sequence_red ;
	ihm->leds[name].seqs[LED_COLOR_GREEN] = sequence_green ;
	ihm->leds[name].seqs[LED_COLOR_BLUE] = sequence_blue ;
}

void IHM_Update(void){

	//Increment the counter each time (since we re called at 1kHz
	compteur ++ ;
	//Update the current color and the current led
	current_led = compteur % LED_COUNT ;

	//Check if we have to change the current bit
	if(compteur > ihm->leds[current_led].next_time)
	{
		ihm->leds[current_led].current_bit >>= 1;
		ihm->leds[current_led].next_time += ihm->leds[current_led].periode ;

		if(ihm->leds[current_led].current_bit == 0)
			ihm->leds[current_led].current_bit = 1 << (SEQUENCE_LEN-1);
	}

	uint16_t pins_high = 0 ;
	uint16_t pins_low = 0 ;
	uint16_t current_bit =  ihm->leds[current_led].current_bit ;

	//For each color, check if we have to set outputs high or low
	if(ihm->leds[current_led].seqs[LED_COLOR_RED] & current_bit)		//Red pin high ?
		pins_high += pin_color[LED_COLOR_RED];
	else
		pins_low += pin_color[LED_COLOR_RED];

	if(ihm->leds[current_led].seqs[LED_COLOR_GREEN] & current_bit)		//Green pin High ?
		pins_high += pin_color[LED_COLOR_GREEN];
	else
		pins_low += pin_color[LED_COLOR_GREEN];

	if(ihm->leds[current_led].seqs[LED_COLOR_BLUE] & current_bit)		//Blue pin high ?
		pins_high |= pin_color[LED_COLOR_BLUE];
	else
		pins_low |= pin_color[LED_COLOR_BLUE];

	//Then we activate the current led
	for(uint8_t l = 0; l < LED_COUNT; l++){
		if(l == current_led)
			pins_high |= ihm->leds[l].pin ;
		else
			pins_low |= ihm->leds[l].pin ;
	}
	HAL_GPIO_WritePin(GPIOC, pins_high, GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC, pins_low, GPIO_PIN_RESET);






}
