/*
 * Mpu.h
 *
 *  Created on: 30 oct. 2021
 *      Author: Théo Magne
 */

#ifndef SENSORS_MPU_H_
#define SENSORS_MPU_H_

#define USE_SPI	0	/* If we dont use SPI, then we use I2C */

#if USE_SPI
#include "../Peripherals/Spi.h"
#define SPI_MPU	spi_e1
#define CS_MPU	cs_eMPU6000
#else
#include "../Peripherals/I2c.h"
#define I2C_MPU i2c_e1
#endif


/* Default I2C address */
#define MPU6050_I2C_ADDR			0xD0

/* MPU6050 registers */
#define MPU6050_AUX_VDDIO			0x01
#define MPU6050_SMPLRT_DIV			0x19
#define MPU6050_CONFIG				0x1A
#define MPU6050_GYRO_CONFIG			0x1B
#define MPU6050_ACCEL_CONFIG		0x1C
#define MPU6050_MOTION_THRESH		0x1F
#define MPU6050_INT_PIN_CFG			0x37
#define MPU6050_INT_ENABLE			0x38
#define MPU6050_INT_STATUS			0x3A
#define MPU6050_ACCEL_XOUT_H		0x3B
#define MPU6050_ACCEL_XOUT_L		0x3C
#define MPU6050_ACCEL_YOUT_H		0x3D
#define MPU6050_ACCEL_YOUT_L		0x3E
#define MPU6050_ACCEL_ZOUT_H		0x3F
#define MPU6050_ACCEL_ZOUT_L		0x40
#define MPU6050_TEMP_OUT_H			0x41
#define MPU6050_TEMP_OUT_L			0x42
#define MPU6050_GYRO_XOUT_H			0x43
#define MPU6050_GYRO_XOUT_L			0x44
#define MPU6050_GYRO_YOUT_H			0x45
#define MPU6050_GYRO_YOUT_L			0x46
#define MPU6050_GYRO_ZOUT_H			0x47
#define MPU6050_GYRO_ZOUT_L			0x48
#define MPU6050_MOT_DETECT_STATUS	0x61
#define MPU6050_SIGNAL_PATH_RESET	0x68
#define MPU6050_MOT_DETECT_CTRL		0x69
#define MPU6050_USER_CTRL			0x6A
#define MPU6050_PWR_MGMT_1			0x6B
#define MPU6050_PWR_MGMT_2			0x6C
#define MPU6050_FIFO_COUNTH			0x72
#define MPU6050_FIFO_COUNTL			0x73
#define MPU6050_FIFO_R_W			0x74
#define MPU6050_WHO_AM_I			0x75
#define MPU6050_READ				0x80

/* Gyroscope sensitivities in °/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)

/* Accelerometer sensitivities in g */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)

/*
 * @brief Accelerometer range enumeration definition
 */
typedef enum
{
	MPU_ACC_2G = 0,
	MPU_ACC_4G,
	MPU_ACC_8G,
	MPU_ACC_16G
} acc_range_e;

/*
 * @brief Gyroscope range enumeration definition
 */
typedef enum
{
	MPU_GYRO_250s = 0,
	MPU_GYRO_500s,
	MPU_GYRO_1000s,
	MPU_GYRO_2000s
} gyro_range_e;

/*
 * @brief MPU State enumeration definition
 */
typedef enum
{
	mpu_state_eOK = 0,
	mpu_state_eERROR
}mpu_state_e;

typedef struct
{
	/* MPU globals */
	mpu_state_e state;
	uint8_t data[14];
	/* Gyroscope */
	gyro_range_e gyro_range;
	uint8_t * gyro_data;
	float gyro_conversion;
	float gyro[3];
	/* Accelerometer */
	uint8_t * acc_data;
	acc_range_e acc_range;
	float acc_conversion;
	float acc[3];
}mpu_t;

void MPU_Init(void);
void MPU_Read_All(void);
float * MPU_Get_Gyro_Ptr(void);
float * MPU_Get_Acc_Ptr(void);
void Gyro_Read(void);

#endif /* SENSORS_MPU_H_ */
