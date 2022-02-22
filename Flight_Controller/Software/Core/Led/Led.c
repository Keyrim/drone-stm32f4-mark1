/*
 * Led.c
 *
 *  Created on: 27 nov. 2021
 *      Author: Théo
 */

#include "Led.h"

#define LED_PERIOD 200      /* duration of a single bit in a sequence (in ms) */
#define SEQUENCE_LENGTH 8   /* length of a sequence in term of bit */

static led_sequence_e current_sequence[led_eCOUNT] = { 0 };
static uint8_t sequence_index[led_eCOUNT] = { 0 };
static uint8_t counter[led_eCOUNT] = { 0 };

/*
 * @brief called by main_ms, turn the led on or off to follow the sequence
 */
void LED_Process_ms(void)
{
	for (uint8_t led = 0; led < led_eCOUNT; led++)
	{
		if (current_sequence[led] != led_sequence_eSEQUENCE_DISABLED)
		{
			counter[led]++;
			if (counter[led] == LED_PERIOD)
			{
				uint8_t current_bit = 1	<< ((SEQUENCE_LENGTH - 1) - sequence_index[led]);
				if (current_sequence[led] & current_bit)
				{
					Led_Set(led, LED_HIGH);
				}
				else
				{
					Led_Set(led, LED_LOW);
				}
				sequence_index[led]++;
				counter[led] = 0;
				if (sequence_index[led] == 8)
				{
					sequence_index[led] = 0;
				}
			}
		}
	}
}

/*
 * @brief Set the state of the led
 * @param state State of the led to set
 */
void Led_Set(led_t led, LED_state_e state)
{
	switch (led)
	{
		case led_eSTATE:
			GPIO_Write_Pin(LED_STATE_GPIO_Port, LED_STATE_Pin, (GPIO_state_e) state);
			break;
		case led_eSTATE2:
			GPIO_Write_Pin(LED_STATE2_GPIO_Port, LED_STATE2_Pin, (GPIO_state_e) state);
			break;
		default:
			break;
	}
}

/*
 * @brief Set the current led sequence
 * @param sequence The new sequence
 */
void Led_Set_Sequence(led_t led, led_sequence_e sequence)
{
	if(current_sequence[led] == sequence)
	{
		return;
	}
	/* Change the current led sequence */
	current_sequence[led] = sequence;
	/* Reset counters to start from the beginning of the sequence */
	counter[led] = 0;
	sequence_index[led] = 0;
}
