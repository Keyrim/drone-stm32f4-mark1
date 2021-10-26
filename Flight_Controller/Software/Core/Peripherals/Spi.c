/*
 * Spi.c
 *
 *  Created on: Oct 25, 2021
 *      Author: Theo
 */

#include "Spi.h"

/*
 * @brief Macro to configure spi structures
 */
#define DEF_SPI(hspi_) 				\
{														\
	.hspi = &hspi_,										\
}														\

#define DEF_CS(cs_port_, cs_pin_) 				\
{												\
	.port = cs_port_,						\
	.pin = cs_pin_							\
}

static cs_t cs[cs_eCOUNT] =
{
		[cs_eMPU6000] = DEF_CS(GPIOA, 12)
};

/*
 * @brief Array of spi which contains every spi
 */
static spi_t spi[spi_eCOUNT] =
{
	[spi_e3] = DEF_SPI(hspi3),
};

/*
 * @brief Initialize hspi with the correct prescaler
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
		HAL_GPIO_WritePin(cs[p].port, cs[p].pin, GPIO_PIN_RESET);
	}

}

