/*
 * ms5611.h
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#ifndef SENSORS_MS5611_H_
#define SENSORS_MS5611_H_

#include "sensors.h"

#define MS5611_ADRESSE  0xEE

/*  MS5611 registers */
#define MS5611_PROM                 0xA0
#define MS5611_TEMP                 0X58
#define MS5611_PRESSURE             0X48
#define MS5611_READ_DATA            0X00

typedef enum ms5611_read_state_e{
	MS5611_READ_TEMPERATURE,
	MS5611_READ_PRESSURE
}ms5611_read_state_e;

typedef struct ms5611_t{

	I2C_HandleTypeDef * hi2c ;
	HAL_StatusTypeDef hal_state ;


	sensor_state_e state ;
	ms5611_read_state_e read_state ;

	uint16_t calibration_values[6];
	uint32_t raw_pressure ;
	uint32_t raw_temperature ;
	uint32_t pressure ;
	int32_t temperature ;
	int32_t dT ;
	float altitude;
	float altitude_shift ;
	uint8_t read_tmp[3];

	//Callback function (data rdy cuz dma complete)
	void (*temp_data)(void);
	void (*pressure_data)(void);


}ms5611_t;


sensor_request_e MS5611_init(ms5611_t * ms5611, I2C_HandleTypeDef * hi2c, void (*temp_data), void (*pressure_data));
sensor_request_e MS5611_request_temp(ms5611_t * ms5611);
sensor_request_e MS5611_request_pressure(ms5611_t * ms5611);
sensor_request_e MS5611_read_temp(ms5611_t * ms5611);
sensor_request_e MS5611_read_pressure(ms5611_t * ms5611);
void MS5611_calculate_temperature(ms5611_t * ms5611);
void MS5611_calculate_pressure(ms5611_t * ms5611);
void MS5611_calculate_altitude(ms5611_t * ms5611);

void MS5611_i2c_rx_callabck(I2C_HandleTypeDef * hi2c);

#endif /* SENSORS_MS5611_H_ */
