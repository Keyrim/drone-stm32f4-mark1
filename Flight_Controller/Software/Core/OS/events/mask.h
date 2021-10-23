/*
 * mask.h
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */

#ifndef EVENTS_MASK_H_
#define EVENTS_MASK_H_

#define NB_ARRAY_MASK 2

#include "../macro_types.h"
#include "flags.h"

typedef struct
{
	uint32_t array[NB_ARRAY_MASK] ;
}Mask_t;

bool_e MASK_set_flag(volatile Mask_t * mask, flag_e flag);
bool_e MASK_clean_flag(volatile Mask_t * mask, flag_e flag);
bool_e Mask_test_and(Mask_t mask_test, Mask_t mask_ref);
bool_e Mask_test_or(Mask_t mask_test, Mask_t mask_ref);
Mask_t MASK_and(Mask_t mask1, Mask_t mask2);
Mask_t MASK_or(Mask_t mask1, Mask_t mask2);
Mask_t MASK_not(Mask_t mask);
Mask_t MASK_create(flag_e * flag_array, int32_t len);
Mask_t MASK_create_single(flag_e flag);
Mask_t MASK_get_empty_mask();
void MASK_clean_every_flags(Mask_t * mask);

#endif /* EVENTS_MASK_H_ */
