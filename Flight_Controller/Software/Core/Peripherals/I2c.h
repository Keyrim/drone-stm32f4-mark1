/*
 * I2c.h
 *
 *  Created on: 11 nov. 2021
 *      Author: Théo Magne
 */

#ifndef I2C_H_
#define I2C_H_

#include "../Inc/i2c.h"

/*
 * @brief I2C enumeration definition
 */
typedef enum
{
	i2c_e1 = 0,
	i2c_eCOUNT
}i2c_e;

/*
 * @brief I2C structure definition
 */
typedef struct
{
	I2C_HandleTypeDef * hi2c;
}i2c_t;

void I2C_Init(void);
HAL_StatusTypeDef I2C_Mem_Write(i2c_e i2c_id, uint8_t device_address, uint8_t mem_address, uint8_t * data, uint8_t size);
HAL_StatusTypeDef I2C_Mem_Read(i2c_e i2c_id, uint8_t device_address, uint8_t mem_address, uint8_t * data, uint8_t size);
HAL_StatusTypeDef I2C_Mem_Read_Dma(i2c_e i2c_id, uint8_t device_address, uint8_t mem_address, uint8_t * data, uint8_t size);

#endif /* I2C_H_ */
