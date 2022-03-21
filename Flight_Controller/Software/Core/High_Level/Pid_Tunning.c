/*
 * Pid_Tunning.c
 *
 *  Created on: 21 mars 2022
 *      Author: Théo Magne
 */

#include "High_Level.h"
#include "../Radio/Radio.h"
#include "../Task_Manager/macro_types.h"


void PID_TUNNING_Main(high_level_t * high_level)
{
	if(high_level->radio[7] < 1500)
	{
		high_level->state = high_level_eIDLE;
		return;

	}
	/* Chose current axe */
	axe_e current_axe;
	if(high_level->radio[6] < 1300)
	{
		current_axe = axe_eROLL;
	}
	else if (high_level->radio[6] < 1600)
	{
		current_axe = axe_ePITCH;
	}
	else
	{
		current_axe = axe_eYAW;
	}
	/* Update kp and ki values */
	CONTROLLER_Get_Pid_KP()[current_axe] -= (float)((high_level->radio[3] - 1500) >> 5)* 0.0002f;
	CONTROLLER_Get_Pid_KI()[current_axe] += (float)((high_level->radio[0] - 1500) >> 5)* 0.0002f;
}
