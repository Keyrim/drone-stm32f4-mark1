/*
 * gyro.c
 *
 *  Created on: Nov 22, 2020
 *      Author: Théo
 */


#include "gyro.h"
#include "../OS/events/events.h"

//float filter[3] = {0.02326917552f, 1.694915254f, -0.7181844298f};
float filter[3] = {0.04f, 1.6f, -0.64f};
static void gyro_data_callback(void);

void GYRO_init(gyro_t * gyro, mpu_t * mpu){

	//Filters init
	FILTER_init(&gyro->filters[ORIENTATION_ROLL], filter, FILTER_SECOND_ORDER);
	FILTER_init(&gyro->filters[ORIENTATION_PITCH], filter, FILTER_SECOND_ORDER);
	FILTER_init(&gyro->filters[ORIENTATION_YAW], filter, FILTER_SECOND_ORDER);

	gyro->mpu = mpu ;
	gyro->raw = gyro->mpu->gyro ;

	gyro->offsets[ORIENTATION_ROLL] = 7.225059382f ;
	gyro->offsets[ORIENTATION_PITCH] = 11.60997625f ;
	gyro->offsets[ORIENTATION_YAW] = -8.234441805f ;

	//Raise the flag "ok" if the gyro get succesfully initiated
	switch(MPU_init_gyro(gyro->mpu, MPU_GYRO_2000s, gyro_data_callback)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_GYRO_OK);
			break;
		case SENSOR_REQUEST_FAIL:
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_update(gyro_t * gyro){
	switch(MPU_update_gyro(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_GYRO_DATA_READY);
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}
bool_e GYRO_calibrate(gyro_t * gyro, uint16_t iteration){
	static uint16_t compteur = 0;

	static float sum[3] = {0};

	if(!compteur){
		sum[0] = 0 ;
		sum[1] = 0 ;
		sum[2] = 0 ;
	}

	sum[0] += gyro->raw[0] + gyro->offsets[0];
	sum[1] += gyro->raw[1] + gyro->offsets[1];
	sum[2] += gyro->raw[2] + gyro->offsets[2];

	compteur ++;

	if(compteur == iteration){
		gyro->offsets[0] = sum[0] / (float)iteration;
		gyro->offsets[1] = sum[1] / (float)iteration;
		gyro->offsets[2] = sum[2] / (float)iteration;
		compteur = 0;
		return TRUE;
	}

	return FALSE;

}
void GYRO_ACC_update(gyro_t * gyro){
	switch(MPU_update_all(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_GYRO_DATA_READY);
			EVENT_Set_flag(FLAG_ACC_DATA_READY);
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_update_dma(gyro_t * gyro){
	switch(MPU_update_gyro_dma(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_ACC_update_dma(gyro_t * gyro){
	switch(MPU_update_all_dma(gyro->mpu)){
		case SENSOR_REQUEST_OK:
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_GYRO_OK);
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void GYRO_process_lpf(gyro_t * gyro){
	//Offset correction first
	gyro->raw[0] -= gyro->offsets[0];
	gyro->raw[1] -= gyro->offsets[1];
	gyro->raw[2] -= gyro->offsets[2];
	//Then the actual low pass filter
	gyro->filtered[0] =  FILTER_process(&gyro->filters[0], gyro->raw[0]);
	gyro->filtered[1] =  FILTER_process(&gyro->filters[1], gyro->raw[1]);
	gyro->filtered[2] =  FILTER_process(&gyro->filters[2], gyro->raw[2]);
	//We warn the system that we ve got new data ready to be used :)
	EVENT_Set_flag(FLAG_GYRO_FILTERED_DATA_READY);
}

static void gyro_data_callback(void){
	EVENT_Set_flag(FLAG_GYRO_DATA_READY);
}






