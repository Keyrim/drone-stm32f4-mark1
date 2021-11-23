/*
 * Mpu.c
 *
 *  Created on: 30 oct. 2021
 *      Author: Théo Magne
 */

#include "Mpu.h"
#include "main.h"

/* MPU6000 use configuration */
static mpu_t mpu =
{
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
	mpu.gyro_data = &mpu.data[8];
	mpu.acc_data = &mpu.data[0];
	/* --------------------- MPU6000 initialization --------------------------- */
#if USE_SPI
	/* Disable I2C */
	uint8_t mpu_disable_i2c[] = {MPU6050_USER_CTRL, 0x10};
	SPI_Transmit(SPI_MPU, CS_MPU, mpu_disable_i2c, 2);
	HAL_Delay(150);
	/* Restart MPU */
	uint8_t wakeup_data[] = {MPU6050_PWR_MGMT_1, 0x80} ;
	SPI_Transmit(SPI_MPU, CS_MPU, wakeup_data, 2);
	HAL_Delay(1000);
	/* Clock select */
	uint8_t mpu_clock_sel_pll_gyroz[] = {MPU6050_PWR_MGMT_1, 0x03};
	SPI_Transmit(SPI_MPU, CS_MPU, mpu_clock_sel_pll_gyroz, 2);
	HAL_Delay(150);
	/* Disable I2C */
	SPI_Transmit(SPI_MPU, CS_MPU, mpu_disable_i2c, 2);
	HAL_Delay(150);
#else
	/* Power up the mpu an wait for it to start */
	HAL_GPIO_WritePin(MPU_POWER_GPIO_Port, MPU_POWER_Pin, GPIO_PIN_SET);
	HAL_Delay(200);
	/* Wake up the mpu */
	uint8_t wakeup_data = 0x80;
	I2C_Mem_Write(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1, &wakeup_data, 1);
	HAL_Delay(15);
	/* Select the clock */
	uint8_t clock_select = 0x03;
	I2C_Mem_Write(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_PWR_MGMT_1, &clock_select, 1);
#endif
	/* -------------------- Gyroscope initialization --------------------------- */
#if USE_SPI
	uint8_t temp_write[2] = { 0 };
	uint8_t temp_read[2] = { 0 };
	temp_write[0] = MPU6050_GYRO_CONFIG | MPU6050_READ ;
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, temp_write, temp_read, 2);
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
	SPI_Transmit(SPI_MPU, CS_MPU, temp_write, 2);
	HAL_Delay(150);
#else
	/* Sensitivity configuration */
	uint8_t temp;
	I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG, &temp, 1);
	temp = (temp & 0xE7) | (uint8_t)mpu.gyro_range << 3;
	I2C_Mem_Write(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG, &temp, 1);
#endif
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
#if USE_SPI
	temp_write[0] = MPU6050_ACCEL_CONFIG | MPU6050_READ ;
	temp_write[1] = 0;
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, temp_write, temp_read, 2);
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
	SPI_Transmit(SPI_MPU, CS_MPU, temp_write, 2);
	HAL_Delay(150);
#else
	/* Sensitivity configuration */
	I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, &temp, 1);
	temp = (temp & 0xE7) | (uint8_t)mpu.acc_range << 3;
	I2C_Mem_Write(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, &temp, 1);
#endif
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
#if USE_SPI
	uint8_t registers [14] = {MPU6050_ACCEL_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, registers, mpu.data, 14);
#else
	I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, mpu.data, 14);
#endif
}

void Gyro_Read(void)
{
#if USE_SPI
	uint8_t registers [6] = {MPU6050_GYRO_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, registers, mpu.gyro_data, 6);
#else
	I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_GYRO_XOUT_H, mpu.gyro_data, 6);
#endif
}





















