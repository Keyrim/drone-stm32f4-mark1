/*
 * I2c.c
 *
 *  Created on: 11 nov. 2021
 *      Author: Théo Magne
 */

#include "I2c.h"

#define I2C_OPERATION_TIMEOUT	10

/*
 * @brief Macro to configure spi structures
 */
#define DEF_I2C(hi2c_) 				\
{														\
	.hi2c = hi2c_,										\
}

static i2c_t i2c[i2c_eCOUNT] =
{
		[i2c_e1] = DEF_I2C(&hi2c1)
};

void I2C_Init(void)
{
}
HAL_StatusTypeDef I2C_Mem_Write(i2c_e i2c_id, uint8_t device_address, uint8_t mem_address, uint8_t * data, uint8_t size)
{
	return HAL_I2C_Mem_Write(i2c[i2c_id].hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, data, size, I2C_OPERATION_TIMEOUT);
}

HAL_StatusTypeDef I2C_Mem_Read(i2c_e i2c_id, uint8_t device_address, uint8_t mem_address, uint8_t * data, uint8_t size)
{
	return HAL_I2C_Mem_Read(i2c[i2c_id].hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, data, size, I2C_OPERATION_TIMEOUT);
}

HAL_StatusTypeDef I2C_Mem_Read_Dma(i2c_e i2c_id, uint8_t device_address, uint8_t mem_address, uint8_t * data, uint8_t size)
{
	return HAL_I2C_Mem_Read_DMA(i2c[i2c_id].hi2c, device_address, mem_address, I2C_MEMADD_SIZE_8BIT, data, size);
}

HAL_StatusTypeDef I2C_Is_Device_Ready(i2c_e i2c_id, uint8_t device_address)
{
	return HAL_I2C_IsDeviceReady(i2c[i2c_id].hi2c, device_address, 10, 10);
}
