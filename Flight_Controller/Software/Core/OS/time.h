/*
 * time.h
 *
 *  Created on: Nov 21, 2020
 *      Author: Théo
 */

#ifndef OS_TIME_H_
#define OS_TIME_H_
#include "stm32f4xx_hal.h"


uint32_t TIME_us(void);
void TIME_delay_us_blocking(uint32_t duration_us);

#endif /* OS_TIME_H_ */
