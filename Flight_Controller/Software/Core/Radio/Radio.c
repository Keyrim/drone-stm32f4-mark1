/*
 * Radio.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */


#include "Radio.h"
#include "Ibus.h"

#define TIMEOUT_RADIO_MS	50

static radio_t radio = { 0 };

void RADIO_Process_Init(void)
{
#if USE_IBUS
	IBUS_Init(radio.channels);
#endif
}

void RADIO_Process_Main(void)
{

#if USE_IBUS
	if(IBUS_Process())
	{
		radio.next_update = HAL_GetTick() + TIMEOUT_RADIO_MS;
		if(radio.state == radio_state_eTIMEOUT)
		{
			radio.counter_is_ok ++;
			if(radio.counter_is_ok > 10)
			{
				radio.state = radio_state_eOK;
			}
		}

	}
	else if(HAL_GetTick() > radio.next_update)
	{
		radio.state = radio_state_eTIMEOUT;
		radio.counter_is_ok = 0;
	}
#endif
}

radio_state_e RADIO_Get_State(void)
{
	return radio.state;
}

uint16_t * RADIO_Get_Channel(void)
{
	return radio.channels;
}


