/*
 * Mpu.c
 *
 *  Created on: 30 oct. 2021
 *      Author: Théo Magne
 */

#include "Mpu.h"
#include "main.h"

#define COEF_GYRO_FILTERING	(0.9f)
#define COEF_ACC_FILTERING	(0.9f)

/* MPU6000 use configuration */
static const mpu_config_t default_mpu_configuration =
{
		.period = 1,
		.gyro_range = MPU_GYRO_2000s,
		.acc_range = MPU_ACC_8G
};

static mpu_t mpu =
{
	.gyro_cali = {0.0136685185f, 0.00779005373f, 0.0112024052f}
};

/* Static functions prototypes */
void MPU_Convert_Acc_Data(void);
void MPU_Convert_Gyro_Data(void);

/*
 * @brief Wake up the MPU6000
 *		  Configure the Gyroscope and the Accelerometer
 */
void MPU_Init(void)
{
	/* Set the configuration */
	mpu.config = default_mpu_configuration;
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
	/* Check is the mpu responds */
	if(I2C_Is_Device_Ready(I2C_MPU, MPU6050_I2C_ADDR))
	{
		mpu.state = mpu_state_eERROR;
		return;
	}
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
	temp_write[1] = (temp_read[1] & 0xE7) | (uint8_t)mpu.config.gyro_range << 3;
	SPI_Transmit(SPI_MPU, CS_MPU, temp_write, 2);
	HAL_Delay(150);
#else
	/* Sensitivity configuration */
	uint8_t temp;
	I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG, &temp, 1);
	temp = (temp & 0xE7) | (uint8_t)mpu.config.gyro_range << 3;
	I2C_Mem_Write(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_GYRO_CONFIG, &temp, 1);
#endif
	switch(mpu.config.gyro_range)
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
	/* deg to rad conversion */
	mpu.gyro_conversion *= 0.0174532925f;
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
	temp = (temp & 0xE7) | (uint8_t)mpu.config.acc_range << 3;
	I2C_Mem_Write(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_ACCEL_CONFIG, &temp, 1);
#endif
	switch(mpu.config.acc_range)
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
float * MPU_Get_Gyro_Ptr(void)
{
	return mpu.gyro;
}

float * MPU_Get_Gyro_Raw_Ptr(void)
{
	return mpu.gyro_raw;
}

float * MPU_Get_Acc_Ptr(void)
{
	return mpu.acc;
}

float * MPU_Get_Acc_Raw_Ptr(void)
{
	return mpu.acc_raw;
}

/*
 * @brief Get the period
 * @return The period in seconds
 */
float MPU_Get_Period(void)
{
	return (float)mpu.config.period * 0.001f;
}

/*
 * @brief Call this function every ms
 * 		  It will send a dma request for the gyro & acc data
 */
void MPU_Process_Ms(void)
{
	if(mpu.counter)
	{
		mpu.counter--;
	}
	else
	{
		mpu.counter = mpu.config.period-1;
		MPU_Read_All_Dma();
	}
}

void MPU_Set_Gyro_Calibration(float * calib)
{
	mpu.gyro_cali[0] += calib[0];
	mpu.gyro_cali[1] += calib[1];
	mpu.gyro_cali[2] += calib[2];
}

void MPU_Read_All(void)
{
	if(mpu.state == mpu_state_eERROR)
	{
		return;
	}
#if USE_SPI
	uint8_t registers [14] = {MPU6050_ACCEL_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, registers, mpu.data, 14);
#else
	if(I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, mpu.data, 14))
	{
		mpu.state = mpu_state_eERROR;
		return;
	}
#endif
	MPU_Convert_Acc_Data();
	MPU_Convert_Gyro_Data();
}

void MPU_Read_All_Dma(void)
{
	if(mpu.state == mpu_state_eERROR)
	{
		return;
	}
#if USE_SPI
	uint8_t registers [14] = {MPU6050_ACCEL_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, registers, mpu.data, 14);
#else
	HAL_StatusTypeDef state = I2C_Mem_Read_Dma(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_ACCEL_XOUT_H, mpu.data, 14);
	if(state)
	{
		mpu.state = mpu_state_eERROR;
		return;
	}
	mpu.dma_state = dma_state_eIN_PORGRESS_ALL;
#endif
}

void Gyro_Read(void)
{
	if(mpu.state == mpu_state_eERROR)
	{
		return;
	}
#if USE_SPI
	uint8_t registers [6] = {MPU6050_GYRO_XOUT_H | MPU6050_READ};
	SPI_Transmit_Receive(SPI_MPU, CS_MPU, registers, mpu.gyro_data, 6);
#else
	if(I2C_Mem_Read(I2C_MPU, MPU6050_I2C_ADDR, MPU6050_GYRO_XOUT_H, mpu.gyro_data, 6))
	{
		mpu.state = mpu_state_eERROR;
		return;
	}
#endif
	MPU_Convert_Gyro_Data();
}

/*
 * @brief Convert acc bytes to measurement
 */
void MPU_Convert_Acc_Data(void)
{
	mpu.acc_raw[0] = ((int16_t)(mpu.acc_data[1] | (mpu.acc_data[0] << 8)) * mpu.acc_conversion);
	mpu.acc_raw[1] = ((int16_t)(mpu.acc_data[3] | (mpu.acc_data[2] << 8)) * mpu.acc_conversion);
	mpu.acc_raw[2] = ((int16_t)(mpu.acc_data[5] | (mpu.acc_data[4] << 8)) * mpu.acc_conversion);
	/* Filtering */
	mpu.acc[0] = (mpu.acc[0] * COEF_ACC_FILTERING) + (mpu.acc_raw[0] * (float)(1-COEF_ACC_FILTERING));
	mpu.acc[1] = (mpu.acc[1] * COEF_ACC_FILTERING) + (mpu.acc_raw[1] * (float)(1-COEF_ACC_FILTERING));
	mpu.acc[2] = (mpu.acc[2] * COEF_ACC_FILTERING) + (mpu.acc_raw[2] * (float)(1-COEF_ACC_FILTERING));
}

/*
 * @brief Convert gyro bytes to measurement
 */
void MPU_Convert_Gyro_Data(void)
{
	/* Data convertion */
	mpu.gyro_raw[0] = ((int16_t)(mpu.gyro_data[1] | (mpu.gyro_data[0] << 8)) * mpu.gyro_conversion) - mpu.gyro_cali[0];
	mpu.gyro_raw[1] = ((int16_t)(mpu.gyro_data[3] | (mpu.gyro_data[2] << 8)) * mpu.gyro_conversion) - mpu.gyro_cali[1];
	mpu.gyro_raw[2] = ((int16_t)(mpu.gyro_data[5] | (mpu.gyro_data[4] << 8)) * mpu.gyro_conversion) - mpu.gyro_cali[2];
	/* Filtering */
	mpu.gyro[0] = (mpu.gyro[0] * COEF_GYRO_FILTERING) + (mpu.gyro_raw[0] * (float)(1-COEF_GYRO_FILTERING));
	mpu.gyro[1] = (mpu.gyro[1] * COEF_GYRO_FILTERING) + (mpu.gyro_raw[1] * (float)(1-COEF_GYRO_FILTERING));
	mpu.gyro[2] = (mpu.gyro[2] * COEF_GYRO_FILTERING) + (mpu.gyro_raw[2] * (float)(1-COEF_GYRO_FILTERING));
}

/*
 * @brief get state of the mpu
 */
bool_e MPU_Is_Ok(void)
{
	return mpu.state == mpu_state_eOK;
}

/*
 * @brief Called back on half rx cplt it
 */
bool_e MPU_Rx_Complete_Callback(void)
{
	if(mpu.dma_state)
	{
		switch(mpu.dma_state)
		{
			case dma_state_eIN_PROGRESS_GYRO:
				MPU_Convert_Gyro_Data();
				break;
			case dma_state_eIN_PROGRESS_ACC:
				MPU_Convert_Acc_Data();
				break;
			case dma_state_eIN_PORGRESS_ALL:
				MPU_Convert_Acc_Data();
				MPU_Convert_Gyro_Data();
				break;
			default:
				break;
		}
		mpu.dma_state = dma_state_eIDLE;
		return TRUE;
	}
	return FALSE;
}





















