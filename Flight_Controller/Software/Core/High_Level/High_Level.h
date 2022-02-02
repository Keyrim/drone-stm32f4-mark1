/*
 * High_level.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo
 */

#ifndef HIGH_LEVEL_H_
#define HIGH_LEVEL_H_

#include "../Task_Manager/macro_types.h"

typedef enum
{
	high_level_eGROUND,
	high_level_eON
}high_level_e;

typedef struct
{
	high_level_e state;
	high_level_e previous_state;
	uint16_t * radio;
	float * power;
	float * target_angle;
	float * target_angle_speed;
}high_level_t;

void HIGH_LEVEL_Init(void);
void HIGH_LEVEL_Process_Main(void);

#endif /* HIGH_LEVEL_H_ */
