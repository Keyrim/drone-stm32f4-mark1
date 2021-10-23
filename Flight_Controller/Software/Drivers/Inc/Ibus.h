/*
 * Ibus.h
 *
 *  Created on: 12 déc. 2020
 *      Author: Théo
 */

#ifndef INC_IBUS_H_
#define INC_IBUS_H_

#include "stm32f4xx_hal.h"
#include "../../Core/OS/macro_types.h"

#define SIZE_DMA_BUFFER 20


typedef enum state_ibus_e{
	STATE_IBUS_LENGTH,
	STATE_IBUS_COMMAND,
	STATE_IBUS_CHANNELS
}state_ibus_e;

typedef struct ibus_t{
	UART_HandleTypeDef * huart ;	//has to be in DMA non circular mode at 115200 bauds
	uint8_t dma_buffer[SIZE_DMA_BUFFER];
	uint8_t index_write ;
	uint8_t index_read ;
	uint16_t channels[14];
	void (*data_rdy_callback)(void);


	state_ibus_e state ;
	uint8_t buffer[32] ;
	uint16_t compteur ;

}ibus_t;

void IBUS_init(ibus_t * ibus_, UART_HandleTypeDef * huart, void(*data_rdy_callback));
bool_e IBUS_Channel_Update(void);
void IBUS_RxCplt(UART_HandleTypeDef * huart);
void IBUS_RxHalfCplt(UART_HandleTypeDef * huart);

#endif /* INC_IBUS_H_ */
