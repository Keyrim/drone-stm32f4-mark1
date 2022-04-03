/*
 * Simulation_Open_Loop.c
 *
 *  Created on: 28 févr. 2022
 *      Author: Théo Magne
 */

#define TEST_DURATION 5000
#define INPUT_POWER	5

#include "High_Level.h"
#include "../Motors/Motors.h"
#include "../Sensors/Mpu.h"
#include "../Radio/Radio.h"

void SIMULATION_OPEN_LOOP_Main(high_level_t * high_level)
{
	static uint8_t state = 0;
	static uint32_t start_time = 0;
	if(high_level->radio[4] < 1500 ||
			!MPU_Is_Ok()||
			RADIO_Get_State() == radio_state_eTIMEOUT)
	{
		high_level->state = high_level_eIDLE;
		state = 0;
	}
	else if(high_level->radio[5] < 1600 && high_level->radio[5] > 1300 && high_level->radio[2] < THROTTLE_MAX_TO_START)
	{
		high_level->state = high_level_eACCRO;
		state = 0;
	}
	else
	{
		switch(state)
		{
			case 0:
				if(high_level->radio[8] >1500)
				{
					start_time = HAL_GetTick();
					float output_motor[4];
					output_motor[orien_control_vector_eMOTOR_FL] = 	 0;
					output_motor[orien_control_vector_eMOTOR_FR] = 	 INPUT_POWER;
					output_motor[orien_control_vector_eMOTOR_BR] = 	 INPUT_POWER;
					output_motor[orien_control_vector_eMOTOR_BL] = 	 0;
					MOTOR_Set(output_motor);
					state = 1;
				}
				break;

			case 1:
				if(HAL_GetTick() >= start_time + TEST_DURATION)
				{
					float output_motor[4] = { 0 };
					MOTOR_Set(output_motor);
					state = 0;
				}
				break;
		}

	}

}

