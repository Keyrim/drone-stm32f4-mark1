/*
 * time.c
 *
 *  Created on: Nov 21, 2020
 *      Author: Théo
 */

#include "time.h"


uint32_t TIME_us(void)
{

	uint32_t t_us;
	static uint32_t previous_t_us = 0;
	__disable_irq();
	t_us = HAL_GetTick() * 1000 + 1000 - SysTick->VAL / 168;
	__enable_irq();

	if(previous_t_us > t_us)
		t_us += 1000;
	previous_t_us = t_us ;

	return t_us;
}
