/*
 * mask.c
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */

#include "mask.h"

/*
 * @brief Return an empty mask
 * return An empty mask
 */
Mask_t MASK_get_empty_mask()
{
	Mask_t mask_return ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
	{
		mask_return.array[m] = 0 ;
	}
	return mask_return ;
}

/*
 * @brief Clear every bits of a flag
 * @param mask Pointer to the mask
 */
void MASK_clean_every_flags(Mask_t * mask)
{
	for(int m = 0; m < NB_ARRAY_MASK; m++)
	{
		mask->array[m] = 0 ;
	}
}

/*
 * @brief Check if every bits in the test mask are present in the ref mask
 * @param mask_test The test mask
 * @param mask_ref The ref mask
 * @return True ...
 */
bool_e Mask_test_and(Mask_t mask_test, Mask_t mask_ref)
{
	for(uint32_t m = 0; m < NB_ARRAY_MASK; m++)
	{
		if((mask_test.array[m] & mask_ref.array[m]) != mask_test.array[m])
		{
			return FALSE ;
		}
	}
	return TRUE ;
}

/*
 * @brief Check if one of the bit in the ref_mask is in the test_mask
 * @param mask_test The test mask
 * @param mask_ref The ref mask
 * @return True if we found one corresponding bit
 */
bool_e Mask_test_or(Mask_t mask_test, Mask_t mask_ref){
	bool_e to_return = FALSE;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
	{
		if(mask_test.array[m] & mask_ref.array[m])
		{
			to_return = TRUE;
		}
	}
	return to_return ;
}

/*
 * @brief Raise the given flag in the given mask
 * @param mask Pointer to the mask
 * @param flag The flag to raise
 * @return False if the flag was already raised True otherwise
 */
bool_e MASK_set_flag(volatile Mask_t * mask, flag_e flag)
{
	uint32_t array_nb = flag / 32;
	uint32_t flag_pos = flag % 32;
	uint32_t flag_mask = (uint32_t)(1 << flag_pos);
	bool_e to_return = TRUE;
	if((mask->array[array_nb] & flag_mask) == flag_mask)
	{
		to_return = FALSE;
	}
	else
	{
		mask->array[array_nb] |= flag_mask;
	}

	return to_return ;
}

/*
 * @brief Clear the given flag in the given mask
 * @param mask Pointer to the mask
 * @param flag The flag to clear
 * @return False if the flag was already cleared True otherwise
 */
bool_e MASK_clear_flag(volatile Mask_t * mask, flag_e flag)
{
	uint32_t array_nb = flag / 32 ;
	uint32_t flag_pos = flag % 32 ;
	uint32_t flag_mask = (uint32_t)(1 << flag_pos) ;
	bool_e to_return = FALSE;
	if((mask->array[array_nb] & flag_mask ) == flag_mask)
	{
		mask->array[array_nb] -= flag_mask ;
		to_return = TRUE;
	}
	return to_return;
}


/*
 * @brief And operation
 * @param mask1 First mask
 * @param mask2 Second mask
 * @return Post operation mask
 */
Mask_t MASK_and(Mask_t mask1, Mask_t mask2){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = mask1.array[m] & mask2.array[m] ;

	return mask_return ;
}

/*
 * @brief Or operation
 * @param mask1 First mask
 * @param mask2 Second mask
 * @return Post operation mask
 */
Mask_t MASK_or(Mask_t mask1, Mask_t mask2){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
	{
		mask_return.array[m] = mask1.array[m] | mask2.array[m];
	}
	return mask_return ;
}

/*
 * @brief Not operation
 * @param mask1 First mask
 * @param mask2 Second mask
 * @return Post operation mask
 */
Mask_t MASK_not(Mask_t mask){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
	{
		mask_return.array[m] = ~mask.array[m] ;
	}
	return mask_return ;
}

/*
 * @brief Create a mask from a flag_array
 * @param flag_array -
 * @param len Length of the array
 * @return The mask created
 */
Mask_t MASK_create(flag_e * flag_array, int32_t len)
{
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t f = 0; f < len; f++)
	{
		MASK_set_flag(&mask_return, flag_array[f]);
	}
	return mask_return ;
}

/*
 * @brief Create a mask from a flag
 * @param flag the flag to create the mask from
 * @return the mask created from the flag
 */
Mask_t MASK_create_single(flag_e flag)
{
	Mask_t mask_return = MASK_get_empty_mask() ;
	MASK_set_flag(&mask_return, flag);
	return mask_return ;
}


