/*
 * Uart.h
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */

#ifndef PERIPHERALS_UART_H_
#define PERIPHERALS_UART_H_


#include "../Inc/usart.h"
#include "../Task_Manager/macro_types.h"

#define LEN_BUFFER	512

/*
 * @brief transmission state enumeration definition
 */
typedef enum
{
	transmission_state_eIDLE,
	transmission_state_eIN_PROGRESS
}transmission_state_e;

/*
 * @brief uart enumeration definition
 */
typedef enum
{
	uart_e1 = 0,
	uart_e3,
	uart_e4,
	uart_eCOUNT,
}uart_e;

/*
 * @brief uart handler structure definition
 */
typedef struct
{
	UART_HandleTypeDef * huart;
	HAL_StatusTypeDef hal_state ;
	transmission_state_e transmission_state ;

	/* Tx stuff */
	uint8_t buffer_tx[LEN_BUFFER];
	uint16_t index_write_tx ;
	uint16_t index_read_tx ;

	/* Rx stuff */
	uint8_t buffer_rx[LEN_BUFFER];
	volatile uint16_t index_write_rx ;	/* Volatile because this is changed inside the uart_idle_line it */
	uint16_t index_read_rx ;
}uart_t;

void UART_Init(void);
void UART_Transfer_Complete_Callback(uart_e id);
void UART_Idle_Line_Callback(uart_e id);
void UART_Transmit(uart_e id, uint8_t * data, uint16_t len);
void UART_Get(uart_e id, uint8_t * c);
uint16_t UART_Availables(uart_e id);


#endif /* PERIPHERALS_UART_H_ */
