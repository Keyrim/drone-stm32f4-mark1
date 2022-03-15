/*
 * Simulation_Open_Loop.c
 *
 *  Created on: 28 févr. 2022
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Motors/Motors.h"
#include "../Sensors/Mpu.h"
#include "../Radio/Radio.h"

void SIMULATION_OPEN_LOOP_Main(high_level_t * high_level)
{
	if(high_level->radio[4] < 1500 ||
			!MPU_Is_Ok()||
			RADIO_Get_State() == radio_state_eTIMEOUT)
	{
		high_level->state = high_level_eIDLE;
	}
	else if(high_level->radio[5] > 1600)
	{
		high_level->state = high_level_eSIMU_CLOSED_LOOP;
	}
	else
	{
		float output_motor[4];
		output_motor[orien_control_vector_eMOTOR_FL] = 	 HL_POWER +  HL_ROLL -  HL_PITCH -  HL_YAW;
		output_motor[orien_control_vector_eMOTOR_FR] = 	 HL_POWER -  HL_ROLL -  HL_PITCH +  HL_YAW;
		output_motor[orien_control_vector_eMOTOR_BR] = 	 HL_POWER -  HL_ROLL +  HL_PITCH -  HL_YAW;
		output_motor[orien_control_vector_eMOTOR_BL] = 	 HL_POWER +  HL_ROLL +  HL_PITCH +  HL_YAW;
		MOTOR_Set(output_motor);
	}

}

