/*
 * Angle.c
 *
 *  Created on: 11 juin 2022
 *      Author: Théo Magne
 */



#include "High_Level.h"
#include "../Motors/Motors.h"
#include "../Sensors/Mpu.h"
#include "../Radio/Radio.h"
#include "../Controller/Controller.h"

void ANGLE_Init(high_level_t * high_level)
{
	CONTROLLER_Set_Mode(controller_mode_eANGLE);
}

void ANGLE_Main(high_level_t * high_level)
{
	if(high_level->radio[4] < 1500 ||
			!MPU_Is_Ok()||
			RADIO_Get_State() == radio_state_eTIMEOUT)
	{
		high_level->state = high_level_eIDLE;
	}
	else if(high_level->radio[5] > 1300)
	{
		high_level->state = high_level_eACCRO;
	}
	else
	{
		high_level->target_angle[axe_eROLL] = ((float)high_level->radio[0] - 1500.0f)*0.006f;
		high_level->target_angle[axe_ePITCH] = ((float)high_level->radio[1] - 1500.0f)*0.006f;
		high_level->target_angle[axe_eYAW] = ((float)high_level->radio[3] - 1500.0f)*0.01f;
		CONTROLLER_Set_Thrust((float)(high_level->radio[2] - 1000));
	}
}
