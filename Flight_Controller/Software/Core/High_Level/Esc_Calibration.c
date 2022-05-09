/*
 * Esc_Calibration.c
 *
 *  Created on: Apr 2, 2022
 *      Author: Théo Magne
 */


#include "High_Level.h"

static float motor_high[4] = { 1000, 1000, 1000, 1000 };
static float motor_low[4] = { 0 };

void ESC_CALIBRATION_Main(high_level_t * high_level)
{
	if(high_level->radio[6] < 1300)
	{
		 high_level->state = high_level_eIDLE;
	}
	else
	{
		if(high_level->radio[4] < 1500)
		{
			MOTOR_Set(motor_low);
		}
		else
		{
			MOTOR_Set(motor_high);
		}
	}
}
