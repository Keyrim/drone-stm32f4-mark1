/*
 * ms5611.c
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#include "ms5611.h"
#include "math.h"
ms5611_t * ms5611_ ;

sensor_request_e MS5611_init(ms5611_t * ms5611, I2C_HandleTypeDef * hi2c, void (*temp_data), void (*pressure_data)){
	ms5611->state = SENSOR_NOT_INIT ;
	ms5611_ = ms5611 ;

	//Callback function
	ms5611->temp_data = temp_data ;
	ms5611->pressure_data = pressure_data ;
	if(hi2c == NULL)
		return SENSOR_REQUEST_FAIL ;

	ms5611->hi2c = hi2c ;
	ms5611->hal_state = HAL_I2C_IsDeviceReady(ms5611->hi2c, MS5611_ADRESSE, 5, 2);
	if(ms5611->hal_state != HAL_OK)
		return SENSOR_REQUEST_FAIL ;

	//Get calibrations values from the sensor
	uint8_t data[2];
	for(uint8_t d = 1; d < 7; d++){
		if(ms5611->hal_state == HAL_OK){
			ms5611->hal_state = HAL_I2C_Mem_Read(ms5611->hi2c, MS5611_ADRESSE, (uint8_t)(MS5611_PROM + (2*d)), I2C_MEMADD_SIZE_8BIT, data, 2, 2);
			ms5611->calibration_values[d-1] = (uint16_t)((data[0] << 8) | data[1]);
		}
		else
			return SENSOR_REQUEST_FAIL ;
	}
	ms5611->state = SENSOR_IDDLE ;

	return SENSOR_REQUEST_OK ;

}

//Demande pour une mesure de température
sensor_request_e MS5611_request_temp(ms5611_t * ms5611){
	if(ms5611->state == SENSOR_BUSY)
		return SENSOR_REQUEST_BUSY;
	if(ms5611->state)
		return SENSOR_REQUEST_FAIL;

	uint8_t temp_request = MS5611_TEMP ;
	ms5611->hal_state = HAL_I2C_Master_Transmit_DMA(ms5611->hi2c, MS5611_ADRESSE, &temp_request, 1);

	if(ms5611->hal_state != HAL_OK){
		ms5611->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL;
	}
	else
		return SENSOR_REQUEST_OK ;
}

//Demande pour une mesure de pressio
sensor_request_e MS5611_request_pressure(ms5611_t * ms5611){
	if(ms5611->state == SENSOR_BUSY)
		return SENSOR_REQUEST_BUSY;
	if(ms5611->state)
		return SENSOR_REQUEST_FAIL;

	uint8_t pressure_request = MS5611_PRESSURE ;
	ms5611->hal_state = HAL_I2C_Master_Transmit_DMA(ms5611->hi2c, MS5611_ADRESSE, &pressure_request, 1);

	if(ms5611->hal_state != HAL_OK){
		ms5611->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL;
	}
	else
		return SENSOR_REQUEST_OK ;
}

//Lecture de la température
sensor_request_e MS5611_read_temp(ms5611_t * ms5611){

	if(ms5611->state == SENSOR_BUSY)
		return SENSOR_REQUEST_BUSY;
	if(ms5611->state)
		return SENSOR_REQUEST_FAIL;

	ms5611->hal_state = HAL_I2C_Mem_Read_DMA(ms5611->hi2c, MS5611_ADRESSE, MS5611_READ_DATA, I2C_MEMADD_SIZE_8BIT, ms5611->read_tmp, 3);
	ms5611->read_state = MS5611_READ_TEMPERATURE ;

	if(ms5611->hal_state != HAL_OK){
		ms5611->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL;
	}
	else
		return SENSOR_REQUEST_OK ;
}

//Lecture de la pression
sensor_request_e MS5611_read_pressure(ms5611_t * ms5611){

	if(ms5611->state == SENSOR_BUSY)
		return SENSOR_REQUEST_BUSY;
	if(ms5611->state)
		return SENSOR_REQUEST_FAIL;


	HAL_I2C_Mem_Read_DMA(ms5611->hi2c, MS5611_ADRESSE, MS5611_READ_DATA, I2C_MEMADD_SIZE_8BIT, ms5611->read_tmp, 3);
	ms5611->read_state = MS5611_READ_PRESSURE ;

	if(ms5611->hal_state != HAL_OK){
		ms5611->state = SENSOR_ERROR ;
		return SENSOR_REQUEST_FAIL;
	}
	else
		return SENSOR_REQUEST_OK ;
}

//Calcul de la température
void MS5611_calculate_temperature(ms5611_t * ms5611){
	ms5611->dT = ms5611->raw_temperature - (int32_t)ms5611->calibration_values[4] * 256;
	ms5611->temperature = 2000 +  ((int64_t) ms5611->dT * ms5611->calibration_values[5]) / 8388608 ;
}

//Calcul de la pression
void MS5611_calculate_pressure(ms5611_t * ms5611){
	 int64_t OFF = (int64_t)ms5611->calibration_values[1] * 65536 + (int64_t)ms5611->calibration_values[3] * ms5611->dT / 128 ;
	 int64_t SENS = (int64_t)ms5611->calibration_values[0] * 32768 + (int64_t)ms5611->calibration_values[2] * ms5611->dT / 256 ;
	 uint64_t pressure = (((uint64_t)ms5611->raw_pressure * SENS / (uint64_t)2097152) - OFF) / (uint64_t)32768 ;
	 ms5611->pressure = (uint32_t)pressure ;
}

//Calcul de l'altitude
void MS5611_calculate_altitude(ms5611_t * ms5611){
	ms5611->altitude = 44330.0f * (1.0 - powf((float)ms5611->pressure * 0.00000986923f, 0.190294957f)) - ms5611->altitude_shift ;
	//Si on a le shift en altitude qui est nul, on l'init
	if(!ms5611->altitude_shift){
		ms5611->altitude_shift = ms5611->altitude ;
		ms5611->altitude = 0;
	}

}


//Callback de l'i2c car dma finit
void MS5611_i2c_rx_callabck(I2C_HandleTypeDef * hi2c){
	UNUSED(hi2c);
	if(ms5611_->read_state == MS5611_READ_TEMPERATURE){
		ms5611_->raw_temperature = (uint32_t)(ms5611_->read_tmp[0]<<16 | ms5611_->read_tmp[1]<<8 | ms5611_->read_tmp[2]);
		ms5611_->temp_data();
	}

	else if(ms5611_->read_state == MS5611_READ_PRESSURE){
		ms5611_->raw_pressure = (uint32_t)(ms5611_->read_tmp[0]<<16 | ms5611_->read_tmp[1]<<8 | ms5611_->read_tmp[2]);
		ms5611_->pressure_data();
	}

}














