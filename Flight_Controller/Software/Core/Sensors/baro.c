/*
 * baro.c
 *
 *  Created on: 2 déc. 2020
 *      Author: Théo
 */

#include "baro.h"
#include "../OS/events/events.h"

//For 1 mesure of temp we do RATIO_PRESSURE_TEMP mesures of pressure
#define RATIO_PRESSURE_TEMP 19

float filter_baro_config [3] = {0.007092198582f, 1.70212766f, -0.7092198582f};
//float filter_baro_config [3] = {0.05f, 0.95f, 0.0f};

float filter_altitude_config[3] = {0.3f, 0.7f, 0.0f};

static void ms5611_raw_temp_rdy();
static void ms5611_raw_pressure_rdy();

static void ms5611_raw_temp_rdy(){
	EVENT_Set_flag(FLAG_BARO_TEMP_RAW_RDY);
}
static void ms5611_raw_pressure_rdy(){
	EVENT_Set_flag(FLAG_BARO_PRESSURE_RAW_RDY);
}

void BARO_init(baro_t * baro, ms5611_t * ms5611, I2C_HandleTypeDef * hi2c){
	baro->ms5611 = ms5611 ;
	baro->pressure_raw = &ms5611->pressure;

	FILTER_init(&baro->filter, filter_baro_config, FILTER_NO_FILTERING);
	FILTER_init(&baro->filter_altitude, filter_altitude_config, FILTER_NO_FILTERING);

	switch(MS5611_init(baro->ms5611, hi2c, ms5611_raw_temp_rdy, ms5611_raw_pressure_rdy))
	{
		case SENSOR_REQUEST_OK :
			EVENT_Set_flag(FLAG_BARO_OK);
			break;
		case SENSOR_REQUEST_FAIL :
			break;
		case SENSOR_REQUEST_BUSY :
			break;
	}
}

uint32_t BARO_Main(baro_t * baro){
	uint32_t to_return = 9600 ;
	switch(baro->state){
		case BARO_STATE_IDDLE:
			baro->state = BARO_STATE_ASK_TEMP ;
			break;
		case BARO_STATE_ASK_TEMP :
			MS5611_request_temp(baro->ms5611);
			baro->state = BARO_STATE_READ_TEMP ;
			break;
		case BARO_STATE_READ_TEMP :
			MS5611_read_temp(baro->ms5611);
			baro->state = BARO_STATE_ASK_PRESSURE ;
			to_return = 400 ;
			break;
		case BARO_STATE_ASK_PRESSURE :
			MS5611_request_pressure(baro->ms5611);
			baro->state = BARO_STATE_READ_PRESSURE;
			break;
		case BARO_STATE_READ_PRESSURE :
			MS5611_read_pressure(baro->ms5611);

			baro->counter_pressure_mesure ++ ;
			baro->counter_pressure_mesure = baro->counter_pressure_mesure % RATIO_PRESSURE_TEMP ;

			if(!baro->counter_pressure_mesure)
				baro->state = BARO_STATE_ASK_TEMP ;
			else
				baro->state = BARO_STATE_ASK_PRESSURE ;

			to_return = 400 ;
			break;
	}
	return to_return ;
}

void BARO_compute_temp(baro_t * baro){
	MS5611_calculate_temperature(baro->ms5611);

}
void BARO_compute_pressure(baro_t * baro){
	MS5611_calculate_pressure(baro->ms5611);
}
void BARO_compute_altitude(baro_t * baro){
	//Filter the pressure
	baro->pressure = FILTER_process(&baro->filter, (float)*baro->pressure_raw);

	baro->altitude_raw = 44330.0f * (1.0 - powf((float)(baro->pressure) * 0.00000986923f, 0.190294957f)) - baro->altitude_offset ;

	baro->altitude = FILTER_process(&baro->filter_altitude, baro->altitude_raw);
	//Si on a le shift en altitude qui est nul, on l'init
	if(!baro->altitude_offset){
		baro->altitude_offset = baro->altitude ;
		baro->altitude = 0;
	}

}


















