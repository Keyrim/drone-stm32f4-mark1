/*
 * Simulation.c
 *
 *  Created on: 26 févr. 2022
 *      Author: Théo Magne
 */

#include "High_Level.h"
#include "../Sensors/Mpu.h"

void SIMULATION_Main(high_level_t * high_level)
{
	if(high_level->radio[9] < 1500)
	{
		if(MPU_Is_Ok())
		{
			high_level->state = high_level_eIDLE;
		}
		else
		{
			high_level->state = high_level_eIDLE_NO_GYRO;
		}
	}
	else
	{
//		float target_roll = (high_level->radio[0]-1500) * 0.2f;
//		high_level->target_angle_speed[axe_eROLL] = target_roll;
		float output_motor[4];
		output_motor[orien_control_vector_eMOTOR_FL] = 	 HL_POWER +  HL_ROLL -  HL_PITCH -  HL_YAW;
		output_motor[orien_control_vector_eMOTOR_FR] = 	 HL_POWER -  HL_ROLL -  HL_PITCH +  HL_YAW;
		output_motor[orien_control_vector_eMOTOR_BR] = 	 HL_POWER -  HL_ROLL +  HL_PITCH -  HL_YAW;
		output_motor[orien_control_vector_eMOTOR_BL] = 	 HL_POWER +  HL_ROLL +  HL_PITCH +  HL_YAW;
		MOTOR_Set(output_motor);
	}
}


