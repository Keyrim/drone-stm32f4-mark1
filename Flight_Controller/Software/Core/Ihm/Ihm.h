/*
 * Ihm.h
 *
 *  Created on: 16 déc. 2020
 *      Author: Théo
 */

#ifndef IHM_IHM_H_
#define IHM_IHM_H_


#include "stm32f4xx_hal.h"
#include "../../Drivers/Inc/buzzer.h"

#define SEQUENCE_LED_1 		0b101000000000
#define SEQUENCE_LED_2 		0b101010111100
#define SEQUENCE_LED_3 		0b101010101010
#define SEQUENCE_LED_4 		0b100010101000
#define SEQUENCE_LED_5 		0b111100000000
#define SEQUENCE_LED_6 		0b100010001000
#define SEQUENCE_LED_7 		0b101010100000
#define SEQUENCE_LED_8 		0b111111000000
#define SEQUENCE_LED_9 		0b001000100010
#define SEQUENCE_LED_10 	0b110000000000
#define SEQUENCE_LED_11 	0b110011000000
#define SEQUENCE_LED_12 	0b110011001100
#define SEQUENCE_LED_ON		0b111111111111
#define SEQUENCE_LED_OFF	0b000000000000


typedef enum led_color_e{
	LED_COLOR_RED,
	LED_COLOR_GREEN,
	LED_COLOR_BLUE,
	LED_COLOR_COUNT
}led_color_e;

typedef enum led_name_e{
	LED_HIGH_LVL,
	LED_SENSORS,
	LED_COUNT
}led_name_e;

typedef enum led_output_logic_e{
	LED_OUTPUT_POSITIVE,
	LED_OUTPUT_NEGATIVE
}led_output_logic_e;

typedef struct rgb_led_t{

	uint16_t seqs [LED_COLOR_COUNT];		//Sequence de 12 bits max
	uint32_t next_time ;
	uint32_t periode ;
	uint16_t pin ;
	uint16_t current_bit ;
	led_output_logic_e output_logic ;

}rgb_led_t;

typedef struct Ihm_t{
	rgb_led_t leds [LED_COUNT];
	buzzer_t buzzer;
}Ihm_t;

void IHM_Init(Ihm_t * ihm_);
void IHM_Led_Init(led_name_e name, uint32_t periode, uint16_t pin, led_output_logic_e output_logic);
void IHM_Set_Sequence(led_name_e name, led_color_e color, uint16_t sequence);
void IHM_Set_Sequences(led_name_e name, uint16_t sequence_red, uint16_t sequence_green, uint16_t sequence_blue);
void IHM_Update(void);

#endif /* IHM_IHM_H_ */
