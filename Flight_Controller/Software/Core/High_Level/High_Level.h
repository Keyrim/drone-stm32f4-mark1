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
	high_level_eIDLE = 0,		/* Startup state */
	high_level_eIDLE_NO_GYRO,
	high_level_eACCRO,
	high_level_eSIMU,
	high_level_eSIMU_OPEN_LOOP,
	high_level_eCOUNT
}high_level_e;

/*
 * @brief Control vector enumeration definition
 */
typedef enum
{
	high_level_control_e1 = 0,
	high_level_control_e2,
	high_level_control_e3,
	high_level_control_e4,
	high_level_control_eCOUNT
}high_level_control_e;

/*
 * @brief general high level structure definition
 */
typedef struct
{
	high_level_e state;
	high_level_e previous_state;
	uint16_t control[high_level_control_eCOUNT];
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
	motor_state_e motor_state;
	orien_mode_e orientation_state;
	void (*entrance)(high_level_t * high_level);
	void (*main)(high_level_t * high_level);
	void (*on_leave)(high_level_t * high_level);
	uint8_t * name;
}high_level_state_t;

void HIGH_LEVEL_Init(void);
void HIGH_LEVEL_Process_Main(void);
void HIGH_LEVEL_Set_Control(high_level_control_e control, uint16_t value);

/* States functions */
void IDLE_Main(high_level_t * high_level);

void IDLE_NO_GYRO_Main(high_level_t * high_level);

void SIMULATION_Main(high_level_t * high_level);

#endif /* HIGH_LEVEL_H_ */
