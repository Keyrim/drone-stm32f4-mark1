/*
 * Motors.h
 *
 *  Created on: Oct 31, 2021
 *      Author: Theo
 */

#ifndef MOTORS_H_
#define MOTORS_H_

#include "../Peripherals/Timer.h"
#include "../Task_Manager/macro_types.h"

/*
 * @brief motor enumeration definition
 */
typedef enum
{
	motor_eFRONT_LEFT 	= 3,
	motor_eFRONT_RIGHT	= 1,
	motor_eBACK_RIGHT	= 0,
	motor_eBACK_LEFT	= 2,
	motor_eCOUNT		= 4
}motor_e;

/*
 * @brief motor state enumeration definition
 */
typedef enum
{
	motor_state_eDISABLED,
	motor_state_eSIMULATION,
	motor_state_eENABLED
}motor_state_e;

/*
 * motor configuration structure definition
 */
typedef struct
{
	uint16_t output_max;
	bool_e use_min_max_check;
	timer_e timer;
}motor_config_t;

typedef struct
{
	motor_config_t config;
	motor_state_e state;
	motor_state_e previous_state;
	float output_float[motor_eCOUNT];
	uint16_t output[motor_eCOUNT];
}motor_t;

void MOTOR_Init(void);
void MOTOR_Process_Ms(void);
void MOTOR_Process_Gyro(void);
void MOTOR_Set_State(motor_state_e new_state);
void MOTOR_Set(float * target);
uint16_t * MOTOR_Get_Output(void);
float * MOTOR_Get_Output_Float(void);
#endif /* MOTORS_H_ */
