/*
 * Idle_no_gyro.c
 *
 *  Created on: 26 févr. 2022
 *      Author: Théo Magne
 */

#include "High_Level.h"
#include "../Radio/Radio.h"

void IDLE_NO_GYRO_Main(high_level_t * high_level)
{
	/* in idle mode we just chill and check for a gyro failure */
	if(high_level->radio[9] > 1500)
	{
		high_level->state = high_level_eSIMU;
	}
}
