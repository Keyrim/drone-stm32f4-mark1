/*
 * Ibus.h
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo MAGNE
 */

#ifndef RADIO_IBUS_H_
#define RADIO_IBUS_H_

#include "Radio.h"

#if USE_IBUS

typedef enum
{
	STATE_IBUS_LENGTH,
	STATE_IBUS_COMMAND,
	STATE_IBUS_CHANNELS
}state_ibus_e;

typedef struct ibus_t
{
	uint16_t * channels;
	uint8_t buffer[32] ;
	uint16_t compteur ;
	state_ibus_e state ;
	uint32_t last_update;
}ibus_t;

void IBUS_Init(uint16_t * channels);
bool_e IBUS_Process(void);

#endif	/* USE_IBUS */



#endif /* RADIO_IBUS_H_ */
