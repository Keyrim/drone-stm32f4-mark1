/*
 * mpu.h
 *
 *  Created on: 26 nov. 2020
 *      Author: Théo
 */

#ifndef SENSORS_MPU_H_
#define SENSORS_MPU_H_

#include "stm32f4xx_hal.h"
#include "sensors.h"
#include "../OS/macro_types.h"

#define NB_MPU_USED 1


#define MPU_USE_SPI



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

/* Gyro sensitivities in °/s */
#define MPU6050_GYRO_SENS_250		((float) 131)
#define MPU6050_GYRO_SENS_500		((float) 65.5)
#define MPU6050_GYRO_SENS_1000		((float) 32.8)
#define MPU6050_GYRO_SENS_2000		((float) 16.4)

/* Acce sensitivities in g */
#define MPU6050_ACCE_SENS_2			((float) 16384)
#define MPU6050_ACCE_SENS_4			((float) 8192)
#define MPU6050_ACCE_SENS_8			((float) 4096)
#define MPU6050_ACCE_SENS_16		((float) 2048)

//Accelerometer range
typedef enum MPU_acc_range_e {
	MPU_ACC_2G = 0x00,
	MPU_ACC_4G = 0x01,
	MPU_ACC_8G = 0x02,
	MPU_ACC_16G = 0x03
} MPU_acc_range_e;

//Gyroscope range
typedef enum MPU_gyro_range_e{
	MPU_GYRO_250s = 0x00,
	MPU_GYRO_500s = 0x01,
	MPU_GYRO_1000s = 0x02,
	MPU_GYRO_2000s = 0x03
} MPU_gyro_range_e;

typedef enum MPU_dma_state_e{
	MPU_DMA_IDDLE,
	MPU_DMA_GYRO_IN_PROGRESS,
	MPU_DMA_ACC_IN_PROGRESS,
	MPU_DMA_ACC_AND_GYRO_IN_PROGRESS
}MPU_dma_state_e;

typedef enum MPU_spi_speed{
	MPU_SPI_SLOW,		//PSC spi equals 64	~600kB/s
	MPU_SPI_FAST		//PSC spi equals 4	~10Mb/s
}MPU_spi_speed;

typedef struct{
	//Hal periph
	I2C_HandleTypeDef * hi2c ;	//Can use I2C
	SPI_HandleTypeDef * hspi ;	//Or spi
	GPIO_TypeDef * pin_cs_gpio ;
	uint16_t pin_cs;
	HAL_StatusTypeDef hal_state ;

	//Mpu global state
	sensor_state_e state ;
	MPU_dma_state_e dma_state;

	//Mpu settings
	uint8_t adresse ;

	//Mpu registers reading
	uint8_t data[14];

	//Gyroscope stuff
	uint8_t * gyro_data;
	int16_t gyro_raw[3];
	float gyro[3];
	MPU_gyro_range_e gyro_range ;
	float gyro_sensi ;
	void (*gyro_data_callback)(void);

	//Accelerometer stuff
	uint8_t * acc_data;
	int16_t acc_raw[3];
	float acc[3];
	MPU_acc_range_e acc_range ;
	float acc_sensi ;
	void (*acc_data_callback)(void);

	//Read register
	uint8_t request_bytes[8];

	// spi speed
	MPU_spi_speed spi_speed ;

	uint32_t start_time_dma ;
	uint32_t end_time_dma ;
	uint32_t delta_time_dma ;
}mpu_t;


//Init functions for the mpu
sensor_request_e MPU_init(mpu_t * mpu, SPI_HandleTypeDef * hspi, GPIO_TypeDef * pin_cs_gpio, uint16_t pin_cs);

sensor_request_e MPU_init_gyro(mpu_t * mpu, MPU_gyro_range_e gyro_range, void (*gyro_data_callback));

sensor_request_e MPU_init_acc(mpu_t * mpu, MPU_acc_range_e acc_range, void (*acc_data_callback));

//Update functions for the mpu
sensor_request_e MPU_update_gyro(mpu_t * mpu);
sensor_request_e MPU_update_gyro_dma(mpu_t * mpu);

sensor_request_e MPU_update_acc(mpu_t * mpu);
sensor_request_e MPU_update_acc_dma(mpu_t * mpu);

sensor_request_e MPU_update_all(mpu_t * mpu);
sensor_request_e MPU_update_all_dma(mpu_t * mpu);








#endif /* SENSORS_MPU_H_ */
