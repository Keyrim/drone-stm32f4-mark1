/*
 * system_d.h
 *
 *  Created on: Oct 23/10/2021
 *      Author: Theo
 */

#ifndef OS_SYSTEM_D_H_
#define OS_SYSTEM_D_H_

#include "macro_types.h"
#include "stm32f4xx_hal.h"

/*
 * @brief software structure definition
 * 		  use this to put things when u do not know where to put them :)
 */
typedef struct
{
	uint8_t cpu_load;
}software_t;

typedef struct
{
	software_t software;
}system_t;


#endif /* OS_SYSTEM_D_H_ */
