/*
 * led_rgb.h
 *
 *  Created on: Dec 3, 2020
 *      Author: Théo
 */

#ifndef INC_LED_RGB_H_
#define INC_LED_RGB_H_

#include "stm32f4xx_hal.h"

#define NB_MAX_LED 2

typedef enum Led_rgb_color_e{
	LED_RGB_COLOR_RED,
	LED_RGB_COLOR_GREEN,
	LED_RGB_COLOR_BLUE
}Led_rgb_color_e;

typedef struct led_rgb_t{

	//Pin localisation
	GPIO_TypeDef * gpio ;
	uint16_t pin_colors[3];

	uint16_t led_pins[NB_MAX_LED];

	uint8_t nb_led ;
}led_rgb_t;

void LED_RGB_init(led_rgb_t * leds, GPIO_TypeDef * gpio, uint16_t * pin_colors, uint16_t * led_pins, uint8_t nb_led);


#endif /* INC_LED_RGB_H_ */
