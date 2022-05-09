 /*
 * macro_types.h
 *
 *  Created on: 30 oct. 2013
 *      Author: Nirgal
 */

#ifndef MACRO_TYPES_H_
#define MACRO_TYPES_H_


#include <stdio.h>


#define UNUSED(X) (void)X

/*
 * @brief boolean enumeration definition
 */
typedef enum
{
	FALSE = 0,
	TRUE
}bool_e;

typedef enum
{
	axe_eROLL = 0,
	axe_ePITCH,
	axe_eYAW,
	axe_eCOUNT
}axe_e;


#define BITS_OFF(var, mask)		((var) &= ~0 ^ (mask))
#define BIT_SET(var, bitno)		((var) |= (1 << (bitno)))
#define BIT_CLR(var, bitno)		((var) &= ~(1 << (bitno)))
#define BIT_TEST(data, bitno)	(((data) >> (bitno)) & 0x01)
#define MIN(a, b)				(((a) > (b)) ? (b) : (a))
#define MAX(a, b)				(((a) > (b)) ? (a) : (b))

#define absolute(x)				(((x) >= 0) ? (x) : (-(x)))


#endif /* MACRO_TYPES_H_ */
