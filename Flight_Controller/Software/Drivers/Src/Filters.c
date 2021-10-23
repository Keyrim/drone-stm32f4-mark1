/*
 * Filter.c
 *
 *  Created on: 20 ao�t 2020
 *      Author: Th�o
 */
#include "../Inc/Filters.h"

void FILTER_init(Filter_t * filter, float settings[3], Filter_order_e order){
	//Param�tre le filtre
	for(int32_t s = 0; s < 3; s++)
		filter->settings[s] = settings[s] ;
	//Mode de filtrage, first order or second order
	filter->order = order ;
}

float FILTER_process(Filter_t * filter, float new_value){
	float to_return = 0 ;
	switch(filter->order){
		case FILTER_FIRST_ORDER :
			filter->values[0] = new_value * filter->settings[0] + filter->values[1] * filter->settings[1] ;
			filter->values[1] = filter->values[0];
			to_return =  filter->values[0];
			break ;

		case FILTER_SECOND_ORDER :
			filter->values[0] = new_value * filter->settings[0] + filter->values[1] * filter->settings[1] + filter->values[2] * filter->settings[2];
			filter->values[2] = filter->values[1];
			filter->values[1] = filter->values[0];
			to_return = filter->values[0];
			break;

		case FILTER_NO_FILTERING :
			to_return = new_value ;
			break;
		default:
			to_return = new_value ;
			break;
	}

	return to_return ;
};

