/*
 * gyro.h
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */

#ifndef SENSORS_GYRO_H_
#define SENSORS_GYRO_H_


//#include "stm32f4xx_hal.h"
#include "sensors.h"
#include "mpu.h"


typedef struct gyro_t{

	//Mpu used
	mpu_t * mpu ;

	//Raw angles
	float * raw ;
	//Filtered angles
	float filtered[3];
	//Calibration values
	float offsets[3];
	//Filters
	Filter_t filters[3];


}gyro_t;

void GYRO_init(gyro_t * gyro, mpu_t * mpu);
void GYRO_update(gyro_t * gyro);
void GYRO_update_dma(gyro_t * gyro);
bool_e GYRO_calibrate(gyro_t * gyro, uint16_t iteration);
void GYRO_process_lpf(gyro_t * gyro);


//Special gyro AND acc functions
void GYRO_ACC_update(gyro_t * gyro);
void GYRO_ACC_update_dma(gyro_t * gyro);

#endif /* SENSORS_GYRO_H_ */
