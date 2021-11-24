/*
 * Controller.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#ifndef CONTROLLER_H_
#define CONTROLLER_H_

#include "Controller.h"
#include "../Task_Manager/macro_types.h"

typedef enum
{
	controller_state_eDISABLED,
	controller_state_eSPEED,
	controller_state_eANGLE
}controller_state_e;

typedef struct
{
	/* Angular position regulation */
	float * angle;							/* 	[degree]		Angles */
	float target_angle[axe_eCOUNT];			/* 	[degree]		Angular target */
	float angle_error[axe_eCOUNT];			/* 	[degree]		Angular error */
	float angle_kp[axe_eCOUNT];				/*					Proportional coef */
	/* Angular speed regulation */
	float * angle_speed;					/*	[degree / s]	Angular speed */
	float target_angle_speed[axe_eCOUNT];	/*	[degree / s]	Angular speed target */
	float angle_speed_error[axe_eCOUNT];	/*	[degree / s]	Angular speed error */
	float angle_speed_kp[axe_eCOUNT];
	/* Controller state */
	float * output;
	controller_state_e state;
	controller_state_e new_state;
}controller_t;

void CONTROLLER_Init(void);
void CONTROLLER_Process(void);
void CONTROLLER_Set_State(controller_state_e new_state);
float * CONTROLLER_Get_Angle_Target(void);
float * CONTROLLER_Get_Angle_Speed_Target(void);

#endif /* CONTROLLER_H_ */
