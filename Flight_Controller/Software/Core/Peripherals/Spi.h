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
	uint16_t pin;
}cs_t;

/*
 * @brief spi enumeration definition
 */
typedef enum
{
	spi_e3,
	spi_eCOUNT
}spi_e;

/*
 * @brief Spi structure definition
 */
typedef struct
{
	SPI_HandleTypeDef * hspi;
}spi_t;

void SPI_Init(void);




#endif /* PERIPHERALS_SPI_H_ */
