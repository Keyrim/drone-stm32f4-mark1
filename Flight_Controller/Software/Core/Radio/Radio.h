/*
 * Radio.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#ifndef RADIO_H_
#define RADIO_H_

#include "../Peripherals/Uart.h"
#include "../../Core/OS/macro_types.h"

#define NB_CHANNEL_MAX 14

#define USE_IBUS

typedef enum
{
	radio_state_eOK,
	radio_state_eTIMEOUT
}radio_state_e;

typedef struct radio_t
{
	uint16_t channels[NB_CHANNEL_MAX];
	radio_state_e state;
}radio_t;

void RADIO_Process(void);

#endif /* RADIO_H_ */
