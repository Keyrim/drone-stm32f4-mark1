/*
 * Led_rgb.c
 *
 *  Created on: Dec 3, 2020
 *      Author: Théo
 */

#include "../Inc/Led_rgb.h"

void LED_RGB_init(led_rgb_t * leds, GPIO_TypeDef * gpio, uint16_t * pin_colors, uint16_t * led_pins, uint8_t nb_led){
	//Gpio set
	leds->gpio = gpio ;
	//Pins color
	leds->pin_colors[LED_RGB_COLOR_BLUE] = 		pin_colors[LED_RGB_COLOR_BLUE] ;
	leds->pin_colors[LED_RGB_COLOR_RED] = 		pin_colors[LED_RGB_COLOR_RED] ;
	leds->pin_colors[LED_RGB_COLOR_GREEN] = 	pin_colors[LED_RGB_COLOR_GREEN] ;
	//Pins leds
	//Number of leds is limited
	nb_led = (nb_led > NB_MAX_LED) ? NB_MAX_LED : nb_led ;
	for(uint8_t l = 0; l < nb_led; l++)
		leds->led_pins[l] = led_pins[l] ;
	//Remember how many leds we have
	leds->nb_led = nb_led ;

}
