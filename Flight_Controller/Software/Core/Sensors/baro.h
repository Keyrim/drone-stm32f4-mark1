/*
 * baro.h
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#ifndef SENSORS_BARO_H_
#define SENSORS_BARO_H_

#include "sensors.h"
#include "ms5611.h"

typedef enum baro_state_e{
	BARO_STATE_IDDLE,
	BARO_STATE_ASK_TEMP,
	BARO_STATE_READ_TEMP,
	BARO_STATE_ASK_PRESSURE,
	BARO_STATE_READ_PRESSURE
}baro_state_e;



typedef struct baro_t{

	//MS5611 used
	ms5611_t * ms5611 ;

	//Variables from the comp used
	int32_t * temperature ;
	uint32_t * pressure_raw ;

	//Filtered variables
	Filter_t filter ;
	float pressure ;


	float altitude_offset ;
	float altitude_raw ;
	float altitude ;
	Filter_t filter_altitude ;


	uint8_t counter_pressure_mesure ;
	baro_state_e state ;




}baro_t;




void BARO_init(baro_t * baro, ms5611_t * ms5611, I2C_HandleTypeDef * hi2c);
void BARO_compute_temp(baro_t * baro);
void BARO_compute_pressure(baro_t * baro);
void BARO_compute_altitude(baro_t * baro);
uint32_t BARO_Main(baro_t * baro);




#endif /* SENSORS_BARO_H_ */
