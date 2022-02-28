/*
 * Idle.c
 *
 *  Created on: 26 févr. 2022
 *      Author: Théo Magne
 */


#include "High_Level.h"
#include "../Sensors/Mpu.h"

void IDLE_Main(high_level_t * high_level)
{
	/* in idle mode we just chill and check for a gyro failure */
	if(!MPU_Is_Ok())
	{
		high_level->state = high_level_eIDLE_NO_GYRO;
	}
	else if(high_level->radio[4] > 1500)
	{
		if(high_level->radio[5] < 1300)
		{
			high_level->state = high_level_eSIMU_OPEN_LOOP;
		}
	}
}
