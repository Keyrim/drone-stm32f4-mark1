/*
 * Supervisor.c
 *
 *  Created on: Nov 27, 2021
 *      Author: Théo
 */

#include "Supervisor.h"
#include "../Sensors/Mpu.h"
#include "../Led/Led.h"
#include "../Radio/Radio.h"

void SUPERVISOR_Process_Ms(void)
{
	if(!MPU_Is_Ok())
	{
		Led_Set_Sequence(led_eSTATE, led_sequence_eGYRO_OFF);
	}
	else if(RADIO_Get_State() == radio_state_eTIMEOUT)
	{
		Led_Set_Sequence(led_eSTATE, led_sequence_eRADIO_TIMEOUT);
	}
	else
	{
		Led_Set_Sequence(led_eSTATE, led_sequence_eOK);
	}
}
