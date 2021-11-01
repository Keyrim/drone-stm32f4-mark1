/*
 * Spi.h
 *
 *  Created on: Oct 25, 2021
 *      Author: Theo
 */

#ifndef PERIPHERALS_SPI_H_
#define PERIPHERALS_SPI_H_

#include "../Inc/spi.h"
#include "../Inc/gpio.h"
#include "../OS/macro_types.h"


/*
 * chip select enumeration definition
 */
typedef enum
{
	cs_eMPU6000,
	cs_eCOUNT
}cs_e;


/*
 * Chip Select structure definition
 */
typedef struct
{
	GPIO_TypeDef * port;
	uint32_t pin_high;
	uint32_t pin_low;
}cs_t;

/*
 * @brief spi enumeration definition
 */
typedef enum
{
	spi_e3,
	spi_eCOUNT,
	/* Rename for a more practical use */
	spi_eMPU6000 = spi_e3
}spi_e;

/*
 * @brief Spi structure definition
 */
typedef struct
{
	SPI_HandleTypeDef * hspi;
}spi_t;

void SPI_Init(void);
void SPI_Transmit(spi_e spi_id, cs_e cs_id, uint8_t * data, uint8_t len);
void SPI_Transmit_Receive(spi_e spi_id, cs_e cs_id, uint8_t * transmit, uint8_t * receive, uint16_t len);




#endif /* PERIPHERALS_SPI_H_ */
