/*
 * Data_logger.h
 *
 *  Created on: Oct 24, 2021
 *      Author: Theo
 */

#ifndef DATA_LOGGER_H_
#define DATA_LOGGER_H_

#include "../OS/system_d.h"

/*
 * @brief data id enumeration definition
 */
typedef enum
{
	data_id_eCONFIG_REQUEST,
	data_id_eSTART_TRANSFER,
	data_id_eSTOP_TRANSFER,
	data_id_eMCU_LOAD,
	data_id_eCOUNT
}data_id_e;


/*
 * @brief Data format enumeration definition
 */
typedef enum
{
	/* 0 byte */
	data_format_e0B_BUTTON = 0,
	/* 1 byte */
	data_format_e8B = 20,
	data_format_e8B_PWM,
	data_format_e8B_FLOAT_1D,
	data_format_e8B_FLOAT_0D,
	/* 2 bytes */
	data_format_e16B = 40,
	data_format_e16B_FLOAT_1D,
	data_format_e16B_FLOAT_2D,
	data_format_e16B_FLOAT_3D,
	data_format_e16B_UINT16,
	data_format_e16B_INT16,
}data_format_e;

/*
 * @brief data format enumeration definition
 * 		  Used to know how to encode the data before transmission
 * 		  Or how decode a data on reception
 */
typedef enum
{
	use_format_eNOT_USED = 0,
	use_format_eAS_OUTPUT,
	use_format_eAS_INPUT
}use_format_e;


typedef struct
{
	uint8_t len;
	void * data;
	uint8_t id;
	data_format_e format;
	uint8_t * name ;
	uint8_t len_name;
	use_format_e use_format;
}data_t;


void DATA_LOGGER_Init(system_t * sys_);
void DATA_LOGGER_Main(void);
void DATA_LOGGER_Reception(uint8_t * input_buffer);
data_t * DATA_LOGGER_Get_Data(data_id_e id);
uint8_t DATA_LOGGER_Get_Data_Value(data_id_e id, uint8_t * buffer);
uint8_t DATA_LOGGER_Get_Data_Config(data_id_e id, uint8_t * buffer);

#endif /* DATA_LOGGER_DATA_LOGGER_H_ */