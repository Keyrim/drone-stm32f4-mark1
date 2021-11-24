/*
 * Ibus.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */


#include "Ibus.h"
#include "../Task_Manager/time.h"

#if USE_IBUS

#define UART_IBUS uart_e4
#define NB_BYTE_MAX_PER_PROCESS		50

static ibus_t ibus = { 0 };

void IBUS_Init(uint16_t * channels)
{
	ibus.channels = channels;
}

uint32_t check_sum_f(uint8_t * buffer);


/*
 * @brief Check for new data on the uart and update channels array
 * @return True when a new have been successfully read
 */
bool_e IBUS_Process(void)
{
	uint16_t nb_to_read = UART_Availables(UART_IBUS);
	if(!nb_to_read)
	{
		return FALSE;
	}
	nb_to_read = MIN(nb_to_read, NB_BYTE_MAX_PER_PROCESS);
	bool_e to_return = FALSE;
	uint8_t c = 0;
	while(nb_to_read)
	{
		UART_Get(UART_IBUS, &c);
		switch(ibus.state)
		{
			case STATE_IBUS_LENGTH :
				if(c == 0x20)
				{
					ibus.state = STATE_IBUS_COMMAND ;
					ibus.buffer[0] = c ;
				}
				break;
			case STATE_IBUS_COMMAND :
				if(c == 0x40)
				{
					ibus.state = STATE_IBUS_CHANNELS ;
					ibus.buffer[1] = c ;
					ibus.compteur = 2 ;
				}
				else if(c != 0x20)
				{
					ibus.state = STATE_IBUS_LENGTH ;
				}
				break ;

			case STATE_IBUS_CHANNELS :
				ibus.buffer[ibus.compteur++] = c ;
				if(ibus.compteur == 32)
				{
					uint32_t cs = (uint32_t)(ibus.buffer[30] | (ibus.buffer[31] << 8)) ;
					if(0xFFFF - check_sum_f(ibus.buffer)== cs)
					{
						for(uint32_t radio = 0; radio < 28; radio +=2)
						{
							ibus.channels[radio / 2] = (int32_t)(ibus.buffer[radio+2] | (ibus.buffer[radio + 3] << 8));
						}
						to_return = TRUE;
					}
					ibus.state = STATE_IBUS_LENGTH ;
				}
				break;
		}
		nb_to_read --;
	}

	return to_return;
}

uint32_t check_sum_f(uint8_t * buffer)
{
	uint32_t sum = 0 ;
	for(uint8_t b=0; b <30; b++)
		sum += (uint32_t)buffer[b];
	return sum ;
}

#endif	/* USE_IBUS */
