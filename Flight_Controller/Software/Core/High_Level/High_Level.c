/*
 * High_Level.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Controller/Controller.h"
#include "../Radio/Radio.h"
#include "../Motor_Mixer/Motor_Mixer.h"
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
	high_level.power = MOTOR_MIXER_Get_Power_Input_Ptr();
	high_level.state = high_level_eGROUND;
	high_level.previous_state = high_level_eON;
}

void HIGH_LEVEL_Process_Main(void)
{
	/* entrance is used to know if we changed of state */
	bool_e entrance = high_level.state != high_level.previous_state;
	high_level.previous_state = high_level.state;
	/* state machine */
	bool_e is_fly_allowed = 1; //MPU_Is_Ok();						/* We need the gyro to fly */
	is_fly_allowed &= high_level.radio[4] > 1500;				/* And the "Arm" switch */
	is_fly_allowed &= (RADIO_Get_State() == radio_state_eOK);	/* We also need a working radio so it doesn't fly away */
	switch(high_level.state)
	{
		case high_level_eGROUND:
			if(entrance)
			{
				CONTROLLER_Set_State(controller_state_eDISABLED);
				MOTOR_Set(0);
			}

			/* Switch 1 and throttle low to arm the drone */
			if(is_fly_allowed && high_level.radio[2] < 1050)
			{
				/* Switch 2 to switch between ANGLE or ACCRO mode */
				if(high_level.radio[5] < 1300)
				{
					high_level.state = high_level_eON;
				}
			}
			break;

		case high_level_eON:{

//			*high_level.power = high_level.radio[2]-1000;
			uint16_t offset = 0;
			if(high_level.radio[7] > 1500)
			{
				offset = 150;
			}
			if(high_level.radio[6] < 1300)
			{
				MOTOR_Set(50 + offset);
			}
			else if(high_level.radio[6] < 1600)
			{
				MOTOR_Set(100 + offset);
			}
			else
			{
				MOTOR_Set(150 + offset);
			}
			/* Can we still fly ? */
			if(!is_fly_allowed)
			{
				high_level.state = high_level_eGROUND;
			}
			break;}
	}
}












