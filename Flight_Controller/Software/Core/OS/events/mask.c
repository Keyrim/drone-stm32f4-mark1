/*
 * mask.c
 *
 *  Created on: 16 sept. 2020
 *      Author: Theo
 */

#include "mask.h"

//Renvoit un mask vide
Mask_t MASK_get_empty_mask(){
	Mask_t mask_return ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = 0 ;
	return mask_return ;
}
//Clean tous les flags d un mask
void MASK_clean_every_flags(Mask_t * mask){
	for(int m = 0; m < NB_ARRAY_MASK; m++)
		mask->array[m] = 0 ;
}

//Fonction qui compare si tous les bits du mask de test sont pr�sent dans le mask de ref
bool_e Mask_test_and(Mask_t mask_test, Mask_t mask_ref){
	//On cherche � savoir si les bits du mask sont pr�sent dans le mask de ref
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		if((mask_test.array[m] & mask_ref.array[m]) != mask_test.array[m]){
			//Si il ne y a pas correspondance sur une des entiers on renvoit faux
			return FALSE ;
		}
	return TRUE ;
}

//Fonction qui compare si au moins des bits du mask de test est pr�sent dans le mask de ref
bool_e Mask_test_or(Mask_t mask_test, Mask_t mask_ref){
	//On cherche � savoir si au moins des bits du mask ref est pr�sent dans le mask de ref
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		if(mask_test.array[m] & mask_ref.array[m])
			//Si un des bits est pr�sent on renvoit vrai
			return TRUE ;
	return FALSE ;
}

//Passe le flag s�l�ctionner � l'�tat haut, renvoit faux si il l'�tait d�j�, vrai sinon
bool_e MASK_set_flag(volatile Mask_t * mask, Flags_e flag){
	uint32_t array_nb = flag / 32 ;
	uint32_t flag_pos = flag % 32 ;
	uint32_t flag_mask = (uint32_t)(1 << flag_pos) ;
	//On v�rifi si le flag est pas d�j� haut
	if((mask->array[array_nb] & flag_mask) == flag_mask)
		return FALSE ;
	//Et on passe � l�tat haut sinon
	mask->array[array_nb] |= flag_mask ;
	return TRUE ;
}

//Clean un flag, renvoit si il �tat d�j� clean, vrai sinon
bool_e MASK_clean_flag(volatile Mask_t * mask, Flags_e flag){
	uint32_t array_nb = flag / 32 ;
	uint32_t flag_pos = flag % 32 ;
	uint32_t flag_mask = (uint32_t)(1 << flag_pos) ;
	//On  v�rifi si le flag est bien lev�
	if((mask->array[array_nb] & flag_mask ) == flag_mask){
		mask->array[array_nb] -= flag_mask ;
		return TRUE ;
	}
	return FALSE ;
}


//Op�rations sur les mask
Mask_t MASK_and(Mask_t mask1, Mask_t mask2){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = mask1.array[m] & mask2.array[m] ;

	return mask_return ;
}
Mask_t MASK_or(Mask_t mask1, Mask_t mask2){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = mask1.array[m] | mask2.array[m] ;

	return mask_return ;
}

Mask_t MASK_not(Mask_t mask){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t m = 0; m < NB_ARRAY_MASK; m++)
		mask_return.array[m] = ~mask.array[m] ;

	return mask_return ;
}



//Cr�er un mask � partir d'un tableau de flag
Mask_t MASK_create(Flags_e * flag_array, int32_t len){
	Mask_t mask_return = MASK_get_empty_mask() ;
	for(int32_t f = 0; f < len; f++)
		MASK_set_flag(&mask_return, flag_array[f]);

	return mask_return ;
}

Mask_t MASK_create_single(Flags_e flag){
	Mask_t mask_return = MASK_get_empty_mask() ;
	MASK_set_flag(&mask_return, flag);
	return mask_return ;
}


