/*
 * Radio.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "../Peripherals/Uart.h"
#include "../Task_Manager/macro_types.h"

#define NB_CHANNEL_MAX 14

#define USE_IBUS 1

typedef enum
{
	radio_state_eOK,
	radio_state_eTIMEOUT
}radio_state_e;

typedef struct radio_t
{
	uint16_t channels[NB_CHANNEL_MAX];
	radio_state_e state;
	uint32_t next_update;
	uint8_t counter_is_ok;
}radio_t;

void RADIO_Process_Main(void);
void RADIO_Process_Init(void);
radio_state_e RADIO_Get_State(void);
uint16_t * RADIO_Get_Channel(void);

#endif /* RADIO_H_ */
