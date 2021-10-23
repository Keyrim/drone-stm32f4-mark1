/*
 * Data_logger.h
 *
 *  Created on: 22 déc. 2020
 *      Author: theom
 */

#ifndef DATA_LOGGER_DATA_LOGGER_H_
#define DATA_LOGGER_DATA_LOGGER_H_

#include "../OS/system_d.h"

typedef enum data_id_e{

	DATA_ID_ROLL_ANGLE,
	DATA_ID_PITCH_ANGLE,

	DATA_ID_ROLL_ACC_ANGLE,
	DATA_ID_PITCH_ACC_ANGLE,

	DATA_ID_ROLL_GYRO,
	DATA_ID_PITCH_GYRO,
	DATA_ID_YAW_GYRO,

	DATA_ID_ROLL_GYRO_RAW,
	DATA_ID_PITCH_GYRO_RAW,
	DATA_ID_YAW_GYRO_RAW,

	DATA_ID_ROLL_ACC,
	DATA_ID_PITCH_ACC,
	DATA_ID_YAW_ACC,

	DATA_ID_ROLL_ACC_RAW,
	DATA_ID_PITCH_ACC_RAW,
	DATA_ID_YAW_ACC_RAW,


	DATA_ID_ACC_X,
	DATA_ID_ACC_Y,
	DATA_ID_ACC_Z,

	DATA_ID_SPEED_X,
	DATA_ID_SPEED_Y,
	DATA_ID_SPEED_Z,

	DATA_ID_POS_X,
	DATA_ID_POS_Y,
	DATA_ID_POS_Z,

	DATA_ID_MOTOR_1,

	DATA_ID_FLIGHT_MODE,

	DATA_ID_CONSIGNE_GYRO_ROLL,
	DATA_ID_CONSIGNE_GYRO_PITCH,
	DATA_ID_CONSIGNE_GYRO_YAW,

	DATA_ID_PID_GYRO_ROLL,
	DATA_ID_PID_GYRO_ROLL_P,
	DATA_ID_PID_GYRO_ROLL_D,
	DATA_ID_PID_GYRO_ROLL_I,

	DATA_ID_PID_GYRO_PITCH,
	DATA_ID_PID_GYRO_YAW,


	DATA_ID_CONSIGNE_ANGLE_ROLL,
	DATA_ID_CONSIGNE_ANGLE_PITCH,

	DATA_ID_CONSIGNE_POS_Z,
	DATA_ID_CONSIGNE_VELOCITY_Z,

	DATA_ID_PID_POS_Z,
	DATA_ID_PID_POS_Z_P,
	DATA_ID_PID_POS_Z_I,
	DATA_ID_PID_POS_Z_D,

	DATA_ID_PID_VELOCITY_Z,
	DATA_ID_PID_VELOCITY_Z_P,
	DATA_ID_PID_VELOCITY_Z_I,
	DATA_ID_PID_VELOCITY_Z_D,

	DATA_ID_PROP_THRUST,
	DATA_ID_MOTOR_FL,
	DATA_ID_MOTOR_FR,
	DATA_ID_MOTOR_BR,
	DATA_ID_MOTOR_BL,

	DATA_ID_PRESSURE,
	DATA_ID_PRESSURE_RAW,
	DATA_ID_TEMPERATURE,
	DATA_ID_ALTITUDE,
	DATA_ID_ALTITUDE_RAW,

	//Buttons
	DATA_ID_CONFIG_REQUEST,		//Si je reçois ça, j'envoit la configuration des données
	DATA_ID_DISABLE_ASSER_ORIENTATION,
	DATA_ID_ENABLE_ASSER_ORIENTATION,
	DATA_ID_START_TRANSFER,
	DATA_ID_STOP_TRANSFER,

	DATA_ID_COUNT
}data_id_e;

typedef enum data_format_e{
	//O octet
	DATA_FORMAT_0B_BUTTON = 0,	//Envoyé par un autre périphérique => déclenche une action sur le drone potentiellement
	//1 octet
	DATA_FORMAT_8B = 20,
	DATA_FORMAT_8B_PWM,
	DATA_FORMAT_8B_FLOAT_1D,
	DATA_FORMAT_8B_FLOAT_0D,
	//2 octets
	DATA_FORMAT_16B = 40,
	DATA_FORMAT_16B_FLOAT_1D,
	DATA_FORMAT_16B_FLOAT_2D,
	DATA_FORMAT_16B_FLOAT_3D,
	DATA_FORMAT_16B_UINT16,
	DATA_FORMAT_16B_INT16,
}data_format_e;


typedef struct data_t{
	uint8_t len;
	uint8_t * data;
	uint8_t id;
	data_format_e format;
	uint8_t * name ;
	uint8_t len_name;
	bool_e used;
}data_t;


void DATA_LOGGER_Init(system_t * sys_);
void DATA_LOGGER_Main(void);
void DATA_LOGGER_Reception(uint8_t * input_buffer);
data_t * DATA_LOGGER_Get_Data(data_id_e id);
uint8_t DATA_LOGGER_Get_Data_Value(data_id_e id, uint8_t * buffer);
uint8_t DATA_LOGGER_Get_Data_Config(data_id_e id, uint8_t * buffer);

#endif /* DATA_LOGGER_DATA_LOGGER_H_ */