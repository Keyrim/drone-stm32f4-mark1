/*
 * batterie.c
 *
 *  Created on: 17 déc. 2020
 *      Author: Théo
 */


#include "batterie.h"


void BATTERIE_Init(batterie_t * batterie, ADC_HandleTypeDef * hadc){
	batterie->hadc = hadc ;
	HAL_ADC_Start_DMA(hadc, batterie->reading, 16);
}
