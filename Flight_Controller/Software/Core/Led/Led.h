/*
 * Led.h
 *
 *  Created on: 27 nov. 2021
 *      Author: Théo
 */

#ifndef LED_H_
#define LED_H_

#include "../Peripherals/Gpio.h"

/*
 * @brief Leds enumeration definition
 */
typedef enum
{
	led_eSTATE = 0,
	led_eSTATE2,
	led_eCOUNT
} led_t;

/*
 * @brief Led HIGH or Led LOW enumeration definition
 */
typedef enum LED_state_e
{
	LED_LOW = 0,
	LED_HIGH
} LED_state_e;

/*
 * @brief led sequences definition
 */
typedef enum led_sequence_e
{
	led_sequence_eSEQUENCE_DISABLED = 0,
	led_sequence_eOK = 0b10000000,
	led_sequence_eGYRO_OFF = 0b10100000,
	led_sequence_eRADIO_TIMEOUT = 0b10101000,
} led_sequence_e;

void Led_Set(led_t led, LED_state_e state);
void LED_Process_ms(void);
void Led_Set_Sequence(led_t led, led_sequence_e sequence);

#endif /* LED_H_ */
