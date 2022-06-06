/*
 * Remote_telemetry.c
 *
 *  Created on: Jun 7, 2022
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Motors/Motors.h"
#include "../Sensors/Mpu.h"
#include "../Radio/Radio.h"
#include "../Controller/Controller.h"


void REMOTE_TELEMETRY_Main(high_level_t * high_level)
{
	if(high_level->radio[4] < 1500 ||
			!MPU_Is_Ok()||
			RADIO_Get_State() == radio_state_eTIMEOUT)
	{
		high_level->state = high_level_eIDLE;
	}
	else if(high_level->radio[5] < 1300)
	{
		high_level->state = high_level_eSIMU_OPEN_LOOP;
	}
	else if(high_level->radio[5] < 1600)
	{
		high_level->state = high_level_eACCRO;
	}
	else
	{
		high_level->target_angle_speed[axe_eROLL] = ((float)high_level->control[0])*0.001f;
		high_level->target_angle_speed[axe_ePITCH] = ((float)high_level->control[1])*0.001f;
		high_level->target_angle_speed[axe_eYAW] = ((float)high_level->control[3])*0.0f;
		CONTROLLER_Set_Thrust(high_level->control[2]);
	}
}
