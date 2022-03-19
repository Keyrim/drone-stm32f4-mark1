/*
 * Simulation_Closed_Loop.c
 *
 *  Created on: 15 mars 2022
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Motors/Motors.h"
#include "../Sensors/Mpu.h"
#include "../Radio/Radio.h"
#include "../Controller/Controller.h"


void SIMULATION_CLOSED_LOOP_Main(high_level_t * high_level)
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
	else
	{
		high_level->target_angle_speed[axe_eROLL] = ((float)high_level->radio[0] - 1500.0f)*0.02f;
		high_level->target_angle_speed[axe_ePITCH] = ((float)high_level->radio[1] - 1500.0f)*0.02f;
		high_level->target_angle_speed[axe_eYAW] = ((float)high_level->radio[3] - 1500.0f)*0.2f;
		CONTROLLER_Set_Thrust((float)(high_level->radio[2] - 1000));
	}
}
