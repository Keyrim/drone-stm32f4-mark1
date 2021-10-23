/*
 * batterie.h
 *
 *  Created on: 17 déc. 2020
 *      Author: Théo
 */

#ifndef SENSORS_BATTERIE_H_
#define SENSORS_BATTERIE_H_

#include "stm32f4xx_hal.h"

typedef struct batterie_t{
	ADC_HandleTypeDef * hadc ;
	uint32_t reading[16];
}batterie_t;

void BATTERIE_Init(batterie_t * batterie, ADC_HandleTypeDef * hadc);

#endif /* SENSORS_BATTERIE_H_ */
