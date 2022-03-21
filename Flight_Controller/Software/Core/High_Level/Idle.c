/*
 * Idle.c
 *
 *  Created on: 26 févr. 2022
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Sensors/Mpu.h"
#include "../Motors/Motors.h"

static float motor_zeros[motor_eCOUNT] = { 0 };

void IDLE_Init(high_level_t * high_level)
{
	MOTOR_Set(motor_zeros);
}

void IDLE_Main(high_level_t * high_level)
{
	/* in idle mode we just chill and check for a gyro failure */
	if(!MPU_Is_Ok())
	{
		high_level->state = high_level_eIDLE_NO_GYRO;
	}
	else if(high_level->radio[4] > 1500  && high_level->radio[2] < THROTTLE_MAX_TO_START)
	{
		if(high_level->radio[5] < 1300)
		{
			high_level->state = high_level_eSIMU_OPEN_LOOP;
		}
		else if(high_level->radio[5] < 1600)
		{
			high_level->state = high_level_eACCRO;
		}
	}
	else if(high_level->radio[7] > 1500)
	{
		high_level->state = high_level_ePID_TUNNING;
	}
}
