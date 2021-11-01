/*
 * Spi.c
 *
 *  Created on: Oct 25, 2021
 *      Author: Theo
 */

#include "Spi.h"


#define SPI_TRANSMIT_TIMEOUT	5

/*
 * @brief Macro to configure spi structures
 */
#define DEF_SPI(hspi_) 				\
{														\
	.hspi = &hspi_,										\
}														\

#define DEF_CS(cs_port_, cs_pin_) 				\
{												\
	.port = cs_port_,							\
	.pin_high = cs_pin_,						\
	.pin_low = cs_pin_<<16						\
}

static cs_t cs[cs_eCOUNT] =
{
		[cs_eMPU6000] = DEF_CS(GPIOA, GPIO_PIN_15)
};

/*
 * @brief Array of spi which contains every spi
 */
static spi_t spi[spi_eCOUNT] =
{
	[spi_e3] = DEF_SPI(hspi3),
};

/*
 * @brief Initialize hspi
 * 		  Set the cs pin in "set" mode to unlock the spi
 */
void SPI_Init(void)
{
	/* Initialize every SPI */
	for(uint8_t s = 0; s < spi_eCOUNT; s++)
	{
		HAL_SPI_Init(spi[s].hspi);
	}
	/* Set every CS pin to "unlock" (set) */
	for(uint8_t p = 0; p < cs_eCOUNT; p++)
	{
		cs[p].port->BSRR = cs[p].pin_high;
	}
}

/*
 * @brief send data to a specified chip over a specified spi
 * @param spi_id Which spi
 * @param cs_id Which chip select
 * @param data data array to transmit
 * @param len number of bytes
 */
void SPI_Transmit(spi_e spi_id, cs_e cs_id, uint8_t * data, uint8_t len)
{
	/* Lock the chip */
	HAL_GPIO_WritePin(cs[cs_id].port, cs[cs_id].pin_high, GPIO_PIN_RESET);
	/* Send data */
	HAL_SPI_Transmit(spi[spi_id].hspi, data, len, SPI_TRANSMIT_TIMEOUT);
	/* Unlock the chip */
	HAL_GPIO_WritePin(cs[cs_id].port, cs[cs_id].pin_high, GPIO_PIN_SET);
}

/*
 * @brief send and receive data to a specified chip over a specified spi
 * @param spi_id Id of the spi to use
 * @param cs_id Id of the chip selet to use
 * @param transmit Data to transmit
 * @param receive Buffer to put the data
 * @param len Number of byte
 */
void SPI_Transmit_Receive(spi_e spi_id, cs_e cs_id, uint8_t * transmit, uint8_t * receive, uint16_t len)
{
	/* Lock the chip */
	HAL_GPIO_WritePin(cs[cs_id].port, cs[cs_id].pin_high, GPIO_PIN_RESET);
	/* Send data */
	HAL_SPI_TransmitReceive(spi[spi_id].hspi, transmit, receive, len, SPI_TRANSMIT_TIMEOUT);
	/* Unlock the chip */
	HAL_GPIO_WritePin(cs[cs_id].port, cs[cs_id].pin_high, GPIO_PIN_SET);
}


















