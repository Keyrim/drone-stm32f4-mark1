/*
 * Telemetry.h
 *
 *  Created on: Dec 18, 2020
 *      Author: Théo
 */

#ifndef TELEMETRY_TELEMETRY_H_
#define TELEMETRY_TELEMETRY_H_



#include "stm32f4xx_hal.h"

//Envoit max de 2000 octets par secondes si utilisation de la radio sinon ça déconne ..
#define USE_RADIO 1



#define SIZE_PORTION_BUFFER 20
#define NB_PORTION_BUFFER	50

#define LEN_RX_BUFFER	512

typedef struct telemetry_t{
	UART_HandleTypeDef * huart;
	HAL_StatusTypeDef hal_state ;

	//Tx stuff
	uint8_t buffer_tx[SIZE_PORTION_BUFFER*NB_PORTION_BUFFER];
	uint8_t len_portion_tx[NB_PORTION_BUFFER];
	uint32_t next_send ;
	uint16_t index_write_tx ;
	uint16_t index_read_tx ;

	//Rx stuff
	uint8_t buffer_rx[LEN_RX_BUFFER];
	uint16_t index_write_rx ;
	uint16_t index_read_rx ;

}telemetry_t;


void TELEMETRY_Init(telemetry_t * telem_, UART_HandleTypeDef * huart);
void TELEMETRY_Process(uint32_t time_us);
void TELEMETRY_Send_Data(uint8_t * data, uint8_t len);
void TELEMETRY_Send_Data_Raw(uint8_t * data, uint8_t len);
void TELEMETRY_Idle_Line_Detection(void);


#endif /* TELEMETRY_TELEMETRY_H_ */
