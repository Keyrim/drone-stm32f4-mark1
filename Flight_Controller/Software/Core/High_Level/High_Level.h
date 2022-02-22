/*
 * High_level.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo
 */

#ifndef HIGH_LEVEL_H_
#define HIGH_LEVEL_H_

#include "../Task_Manager/macro_types.h"
#include "../Motors/Motors.h"
#include "../Controller/Controller.h"
#include "../System/Orientation.h"

/*
 * @brief high level state enumeration definition
 */
typedef enum
{
	high_level_eGROUND,
	high_level_eACCRO,
	high_level_eSIMU,
	high_level_eCOUNT
}high_level_e;

/*
 * @brief general high level structure definition
 */
typedef struct
{
	high_level_e state;
	high_level_e previous_state;
	uint16_t * radio;
	float * power;
	float * target_angle;
	float * target_angle_speed;
}high_level_t;

/*
 * @brief high level's states structure definition
 */
typedef struct
{
	controller_state_e controller_state;
	void (*init)(void);
	void (*main)(void);
	void (*on_leave)(void);
	uint8_t * name;
}high_level_state_t;

void HIGH_LEVEL_Init(void);
void HIGH_LEVEL_Process_Main(void);

#endif /* HIGH_LEVEL_H_ */
