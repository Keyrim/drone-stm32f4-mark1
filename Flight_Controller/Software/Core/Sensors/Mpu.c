/*
 * Mpu.c
 *
 *  Created on: 30 oct. 2021
 *      Author: Theo
 */

#include "Mpu.h"

/* MPU6000 use configuration */
static mpu_t mpu =
{
		.cs = cs_eMPU6000,
		.spi = spi_eMPU6000,
		.gyro_range = MPU_GYRO_2000s,
		.acc_range = MPU_ACC_8G
};

/*
 * @brief Wake up the MPU6000
 *		  Configure the Gyroscope and the Accelerometer
 */
void MPU_Init(void)
{
	/* Set the Gyroscope and Accelerometer data buffer */
	mpu.gyro_data = &mpu.readings[8];
	mpu.acc_data = &mpu.readings[0];
	/* --------------------- MPU6000 initialization --------------------------- */
	/* Disable I2C */
	uint8_t mpu_disable_i2c[] = {MPU6050_USER_CTRL, 0x10};
	SPI_Transmit(mpu.spi, mpu.cs, mpu_disable_i2c, 2);
	HAL_Delay(150);
	/* Restart MPU */
	uint8_t wakeup_data[] = {MPU6050_PWR_MGMT_1, 0x80} ;
	SPI_Transmit(mpu.spi, mpu.cs, wakeup_data, 2);
	HAL_Delay(1000);
	/* Clock select */
	uint8_t mpu_clock_sel_pll_gyroz[] = {MPU6050_PWR_MGMT_1, 0x03};
	SPI_Transmit(mpu.spi, mpu.cs, mpu_clock_sel_pll_gyroz, 2);
	HAL_Delay(150);
	/* Disable I2C */
	SPI_Transmit(mpu.spi, mpu.cs, mpu_disable_i2c, 2);
	HAL_Delay(150);
	/* -------------------- Gyroscope initialization --------------------------- */
	uint8_t temp_write[2] = { 0 };
	uint8_t temp_read[2] = { 0 };
	temp_write[0] = MPU6050_GYRO_CONFIG | MPU6050_READ ;
	SPI_Transmit_Receive(mpu.spi, mpu.cs, temp_write, temp_read, 2);
	HAL_Delay(150);
	/* If there is no answer, then there is no MPU ..*/
	if(temp_read[0] == 0 || temp_read[1] == 0)
	{
		mpu.state = mpu_state_eNO_ANSWER;
		return;
	}
	/* Sensitivity configuration */
	temp_write[0] = MPU6050_GYRO_CONFIG;
	temp_write[1] = (temp_read[1] & 0xE7) | (uint8_t)mpu.gyro_range << 3;
	SPI_Transmit(mpu.spi, mpu.cs, temp_write, 2);
	HAL_Delay(150);
	switch(mpu.gyro_range)
	{
		case MPU_GYRO_250s :
			mpu.gyro_conversion = (float)1 / MPU6050_GYRO_SENS_250 ;
			break;
		case MPU_GYRO_500s :
			mpu.gyro_conversion = (float)1 / MPU6050_GYRO_SENS_500 ;
			break;
		case MPU_GYRO_1000s :
			mpu.gyro_conversion = (float)1 / MPU6050_GYRO_SENS_1000 ;
			break;
		case MPU_GYRO_2000s :
			mpu.gyro_conversion = (float)1 / MPU6050_GYRO_SENS_2000 ;
			break;
	}
	/* -------------------- Accelerometer initialization --------------------------- */
	temp_write[0] = MPU6050_ACCEL_CONFIG | MPU6050_READ ;
	temp_write[1] = 0;
	SPI_Transmit_Receive(mpu.spi, mpu.cs, temp_write, temp_read, 2);
	/* If there is no answer, then there is no MPU ..*/
	if(temp_read[0] == 0 || temp_read[1] == 0)
	{
		mpu.state = mpu_state_eNO_ANSWER;
		return;
	}
	HAL_Delay(150);
	/* Sensitivity configuration */
	temp_write[0] = MPU6050_ACCEL_CONFIG;
	temp_write[1] = (temp_read[1] & 0xE7) | (uint8_t)mpu.acc_range << 3;
	SPI_Transmit(mpu.spi, mpu.cs, temp_write, 2);
	HAL_Delay(150);
	switch(mpu.acc_range)
	{
		case MPU_ACC_2G :
			mpu.acc_conversion = (float)1 / MPU6050_ACCE_SENS_2 ;
			break;
		case MPU_ACC_4G :
			mpu.acc_conversion = (float)1 / MPU6050_ACCE_SENS_4 ;
			break;
		case MPU_ACC_8G :
			mpu.acc_conversion = (float)1 / MPU6050_ACCE_SENS_8 ;
			break;
		case MPU_ACC_16G :
			mpu.acc_conversion = (float)1 / MPU6050_ACCE_SENS_16 ;
			break;
	}
}
void MPU_Read_All(void)
{
	uint8_t registers [14] = {MPU6050_ACCEL_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(mpu.spi, mpu.cs, registers, mpu.readings, 14);
}

void Gyro_Read(void)
{
	uint8_t registers [6] = {MPU6050_GYRO_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(mpu.spi, mpu.cs, registers, mpu.gyro_data, 6);
}
