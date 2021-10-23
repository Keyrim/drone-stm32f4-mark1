/*
 * filters.h
 *
 *  Created on: 20 ao�t 2020
 *      Author: Th�o
 */

#ifndef BTM_FILTERS_H_
#define BTM_FILTERS_H_

#include "stm32f4xx_hal.h"
#include "../../Core/OS/macro_types.h"


typedef enum {
	FILTER_FIRST_ORDER = 0,
	FILTER_SECOND_ORDER,
	FILTER_NO_FILTERING
}Filter_order_e;

//Filter structure
typedef struct{
	float settings[3];
	float values[3];
	Filter_order_e order ;
}Filter_t;


void FILTER_init(Filter_t * filter, float settings[3], Filter_order_e order);
float FILTER_process(Filter_t * filter, float new_value);


#endif /* BTM_FILTERS_H_ */
