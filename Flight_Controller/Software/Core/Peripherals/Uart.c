/*
 * Uart.c
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */


#include "Uart.h"


/* -------------------------- Privates variables -------------------------- */
static uart_t uart[uart_eCOUNT] = { 0 };

/* -------------------------- Privates functions prototypes -------------------------- */
uint16_t get_nb_bytes_available(uart_e id);

/* -------------------------- Public callback functions -------------------------- */
/*
 * @brief Update the read index when the idle flag is raised
 */
void UART_Idle_Line_Callback(uart_e id)
{
	if(__HAL_UART_GET_FLAG(uart[id].huart, UART_FLAG_IDLE))
	{
		uart[id].index_write_rx = LEN_BUFFER - uart[id].huart->hdmarx->Instance->NDTR ;
	  __HAL_UART_CLEAR_IDLEFLAG(uart[id].huart);
	}
}

/*
 * @brief Re start the dma transfer if there is data to send or reset the transmission state to idle
 */
void UART_Transfer_Complete_Callback(uart_e id)
{
	if(uart[id].index_write_tx != uart[id].index_read_tx)
	{
		uint16_t nb_bytes_to_read = 0;
		if(uart[id].index_write_tx > uart[id].index_read_tx)
		{
			nb_bytes_to_read = uart[id].index_write_tx - uart[id].index_read_tx;
		}
		else
		{
			nb_bytes_to_read = LEN_BUFFER - uart[id].index_read_tx;
		}
		HAL_UART_Transmit_DMA(uart[id].huart, &uart[id].buffer_tx[uart[id].index_read_tx], nb_bytes_to_read);
		uart[id].index_read_tx = (uart[id].index_read_tx + nb_bytes_to_read) % LEN_BUFFER;
	}
	else
	{
		uart[id].transmission_state = transmission_state_eIDLE;
	}
}

/*
 * @brief Number of byte available in the RX buffer
 * @param id Id of the uart we are interested in
 * @return Number of bytes
 */
uint16_t UART_Availables(uart_e id)
{
	return get_nb_bytes_available(id);
}

/*
 * @brief Write the next available character in the buffer
 * 		  Does nothing if there are no characters available
 * @param id Id of the uart
 * @param c Buffer to write the character into
 */
void UART_Get(uart_e id, uint8_t * c)
{
	if(get_nb_bytes_available(id))
	{
		*c = uart[id].buffer_rx[uart[id].index_read_rx];
		uart[id].index_read_rx = (uart[id].index_read_rx + 1) % LEN_BUFFER;
	}
}

/* -------------------------- Public functions -------------------------- */

/*
 * @brief Initialize every uarts and start dma reception
 */
void UART_Init(void)
{
	/* Link  huarts to the right uarts structures */
	uart[uart_e1].huart = &huart1;
	uart[uart_e3].huart = &huart3;
	uart[uart_e4].huart = &huart4;

	/* Initialization */
	for(uint8_t u = 0; u < uart_eCOUNT; u ++)
	{
		__HAL_UART_ENABLE_IT(uart[u].huart, UART_IT_IDLE);
		HAL_UART_Receive_DMA(uart[u].huart, uart[u].buffer_rx, LEN_BUFFER);
	}
}



/*
 * @brief Transmit data in dma on the given uart
 * @param id Id of the uart to use
 * @param data Array with the data
 * @param len Length of the data array
 */
void UART_Transmit(uart_e id, uint8_t * data, uint16_t len)
{
	for(uint16_t c = 0; c < len; c ++)
	{
		uart[id].buffer_tx[(uart[id].index_write_tx + c) % LEN_BUFFER] = data[c];
	}
	uart[id].index_write_tx = (uart[id].index_write_tx + len) % LEN_BUFFER;
	if(uart[id].transmission_state == transmission_state_eIDLE)
	{
		uart[id].transmission_state = transmission_state_eIN_PROGRESS;
		uint16_t nb_bytes_before_end_of_buffer = LEN_BUFFER - uart[id].index_read_tx;
		len = MIN(len, nb_bytes_before_end_of_buffer);
		HAL_UART_Transmit_DMA(uart[id].huart, &uart[id].buffer_tx[uart[id].index_read_tx], len);
		uart[id].index_read_tx = (uart[id].index_read_tx + len) % LEN_BUFFER;
	}
}

/*
 * @brief Compute the numbers of byte available in the buffer
 * @param id Id of the uart
 * @return nb of bytes available
 */
uint16_t get_nb_bytes_available(uart_e id)
{
	uint16_t nb = 0 ;
	if(uart[id].index_write_rx > uart[id].index_read_rx)
	{
		nb = uart[id].index_write_rx - uart[id].index_read_rx ;
	}
	else if(uart[id].index_write_rx < uart[id].index_read_rx)
	{
		nb = LEN_BUFFER - uart[id].index_read_rx + uart[id].index_write_rx ;
	}
	return nb;
}







