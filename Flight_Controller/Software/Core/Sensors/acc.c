/*
 * acc.c
 *
 *  Created on: 1 déc. 2020
 *      Author: Theo
 */


#include "acc.h"
#include "../OS/events/events.h"

static void acc_data_callback(void);

float filter_config [3] = {0.04f, 1.6f, -0.64f};
//float filter_config [3] = {0.002267573696f, 1.904761905f, -0.9070294785f};
//float filter_config [3] = {0.1f, 0.9f, 0.0f};

void ACC_init(acc_t * acc, mpu_t * mpu){

	FILTER_init(&acc->filters[ACC_AXE_X], filter_config, FILTER_SECOND_ORDER);
	FILTER_init(&acc->filters[ACC_AXE_Y], filter_config, FILTER_SECOND_ORDER);
	FILTER_init(&acc->filters[ACC_AXE_Z], filter_config, FILTER_SECOND_ORDER);


	acc->mpu = mpu ;
	acc->raw = acc->mpu->acc ;

	acc->offsets[0] = 0.0203813482 ;
	acc->offsets[1] = -0.0299842935 ;
	acc->offsets[2] = -0.16 ;

	switch(MPU_init_acc(acc->mpu, MPU_ACC_4G, acc_data_callback)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_FAIL:
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}

}

void ACC_update(acc_t * acc){
	switch(MPU_update_acc(acc->mpu)){
		case SENSOR_REQUEST_OK:
			EVENT_Set_flag(FLAG_ACC_DATA_READY);
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

bool_e ACC_calibrate(acc_t * acc, uint16_t iteration){
	static uint16_t compteur = 0;

	static float sum[3] = {0};

	if(!compteur){
		sum[0] = 0 ;
		sum[1] = 0 ;
		sum[2] = 0 ;
	}

	sum[0] += acc->raw[0] + acc->offsets[0];
	sum[1] += acc->raw[1] + acc->offsets[1];
	sum[2] += acc->raw[2] + acc->offsets[2] - 1;	//Pour le YAW on doit avoir 1g quand le drone est à l'horizontale

	compteur ++;

	if(compteur == iteration){
		acc->offsets[0] = sum[0] / (float)iteration;
		acc->offsets[1] = sum[1] / (float)iteration;
		acc->offsets[2] = sum[2] / (float)iteration;
		compteur = 0;
		return TRUE;
	}

	return FALSE;

}

void ACC_update_dma(acc_t * acc){
	switch(MPU_update_acc_dma(acc->mpu)){
		case SENSOR_REQUEST_OK:
			break;
		case SENSOR_REQUEST_FAIL:
			EVENT_Clean_flag(FLAG_ACC_OK);
			break;
		case SENSOR_REQUEST_BUSY:
			break;
	}
}

void ACC_process_lpf(acc_t * acc){
	//Offset correction first
	acc->raw[0] -= acc->offsets[0];
	acc->raw[1] -= acc->offsets[1];
	acc->raw[2] -= acc->offsets[2];
	//Then the actual low pass filter
	acc->filtered[ACC_AXE_X] = FILTER_process(&acc->filters[ACC_AXE_X], acc->raw[ACC_AXE_X]);
	acc->filtered[ACC_AXE_Y] = FILTER_process(&acc->filters[ACC_AXE_Y], acc->raw[ACC_AXE_Y]);
	acc->filtered[ACC_AXE_Z] = FILTER_process(&acc->filters[ACC_AXE_Z], acc->raw[ACC_AXE_Z]);
	//We warn the system that new data are available
	EVENT_Set_flag(FLAG_ACC_FILTERED_DATA_READY);
}

static void acc_data_callback(void){
	EVENT_Set_flag(FLAG_ACC_DATA_READY);
}






