/*
 * High_Level.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Controller/Controller.h"
#include "../Radio/Radio.h"
#include "../Sensors/Mpu.h"

#define RADIO_TO_ANGLE			((float)0.1f)
#define RADIO_TO_ANGLE_RATE 	((float)0.5f)
#define RADIO_TO_ANGLE_RATE_YAW	((float)0.7f)

static high_level_t high_level = { 0 };

void HIGH_LEVEL_Init(void)
{
	high_level.radio = RADIO_Get_Channel();
	high_level.target_angle = CONTROLLER_Get_Angle_Target();
	high_level.target_angle_speed = CONTROLLER_Get_Angle_Speed_Target();
	high_level.state = high_level_eGROUND;
	high_level.previous_state = high_level_eACCRO;
}

void HIGH_LEVEL_Process_Main(void)
{
	/* entrance is used to know if we changed of state */
//	bool_e entrance = high_level.state == high_level.previous_state;
//	high_level.previous_state = high_level.state;
//	/* state machine */
//	bool_e is_fly_allowed = MPU_Is_Ok();						/* We need the gyro to fly */
//	is_fly_allowed &= high_level.radio[4] > 1500;				/* And the "Arm" switch */
//	is_fly_allowed &= (RADIO_Get_State() == radio_state_eOK);	/* We also need a working radio so it doesn't fly away */
//	switch(high_level.state)
//	{
//		case high_level_eGROUND:
//			if(entrance)
//			{
//				CONTROLLER_Set_State(controller_state_eDISABLED);
//				*high_level.power = 0;
//			}
//
//			/* Switch 1 and throttle low to arm the drone */
//			if(is_fly_allowed && high_level.radio[2] < 1050)
//			{
//				/* Switch 2 to switch between ANGLE or ACCRO mode */
//				if(high_level.radio[5] < 1300)
//				{
//					high_level.state = high_level_eANGLE;
//				}
//				else if(high_level.radio[5] > 1300)
//				{
//					high_level.state = high_level_eACCRO;
//				}
//			}
//			break;
//
//		case high_level_eANGLE:
//			if(entrance)
//			{
//				CONTROLLER_Set_State(controller_state_eANGLE);
//			}
//			high_level.target_angle[axe_eROLL] = (float)(high_level.radio[0]-1500) * RADIO_TO_ANGLE;
//			high_level.target_angle[axe_ePITCH] = (float)(high_level.radio[1]-1500) * RADIO_TO_ANGLE;
//			high_level.target_angle_speed[axe_eYAW] = -(float)(high_level.radio[3]-1500) * RADIO_TO_ANGLE_RATE_YAW;
//			*high_level.power = high_level.radio[2]-1000;
//			/* Can we still fly ? */
//			if(!is_fly_allowed)
//			{
//				high_level.state = high_level_eGROUND;
//			}
//			/* Switch 2 to go in angle mode */
//			else if(high_level.radio[5] > 1300)
//			{
//				high_level.state = high_level_eACCRO;
//			}
//			break;
//		case high_level_eACCRO:
//			if(entrance)
//			{
//				CONTROLLER_Set_State(controller_state_eSPEED);
//			}
//			high_level.target_angle_speed[axe_eROLL] = (float)(high_level.radio[0]-1500) * RADIO_TO_ANGLE_RATE;
//			high_level.target_angle_speed[axe_ePITCH] = (float)(high_level.radio[1]-1500) * RADIO_TO_ANGLE_RATE;
//			high_level.target_angle_speed[axe_eYAW] = -(float)(high_level.radio[3]-1500) * RADIO_TO_ANGLE_RATE_YAW;
//			*high_level.power = high_level.radio[2]-1000;
//			/* Can we still fly ? */
//			if(!is_fly_allowed)
//			{
//				high_level.state = high_level_eGROUND;
//			}
//			/* Switch 2 to go in angle mode */
//			else if(high_level.radio[5] < 1300)
//			{
//				high_level.state = high_level_eANGLE;
//			}
//			break;
//	}
}












