/*
 * Data_logger.c
 *
 *  Created on: Oct 24, 2021
 *      Author: Theo
 */

#include "Data_logger.h"
#include "string.h"
#include "../Peripherals/Uart.h"

/* The files needed to include the data must be included */
#include "../Sensors/Mpu.h"
#include "../Complementary_Filter/Complementary_Filter.h"
#include "../Radio/Radio.h"
#include "../Peripherals/Timer.h"
#include "../Motors/Motors.h"
#include "../Task_Manager/Task_Manager.h"
#include "../Controller/Controller.h"
#include "../High_Level/High_Level.h"
#include "../Motor_Mixer/Motor_Mixer.h"


#define PERIODE_PING 			500
#define PERIODE_CONFIG_SEND 	10
#define PERIODE_SEND			1
#define UART_TELEMETRY			uart_e1

/*
 * @brief Macro to define data
 */
#define DEFINE_DATA(id_, ptr_, format_, name_, use_) 	\
{														\
	data_list[id_].id = id_ ;							\
	data_list[id_].data = ptr_ ;						\
	data_list[id_].format = format_;					\
	data_list[id_].name = (uint8_t*)name_;				\
	data_list[id_].len_name = sizeof(name_)-1;			\
	data_list[id_].use_format = use_;					\
}

/*
 * @brief Data logger state enumeration definition
 * 		  Used by the data logger state machine in the main function
 */
typedef enum
{
	data_logger_state_eIDLE,
	data_logger_state_eTRANSMIT_CONFIG,
	data_logger_state_eLOG
}data_logger_state_e;

/*
 * @brief uart parser state enumeration definition
 */
typedef enum
{
	parser_state_eSTART_BYTE,
	parser_state_eID,
	parser_state_eDATA,
	parser_state_eCHECKSUM
}parser_state_e;


/* ----------------- Privates variables ----------------- */
/* Array of every data */
static data_t data_list[data_id_eCOUNT] ;
/* Flags */
static bool_e start_flag = FALSE;
static bool_e send_config_flag = FALSE ;
static bool_e stop_flag = FALSE;
/* Data operations */
static uint8_t tmp[50];
static uint8_t tmp_len ;
static uint8_t id_init_counter = 0;
/* Used somewhere */
static bool_e is_any_data_used = FALSE;
static bool_e config_not_sent = TRUE ;


/* ----------------- Privates functions prototypes ----------------- */
static void push_data_to_uart(uint8_t * buffer, uint8_t len);
static void state_machine(void);
static void parse_uart(void);
/*
 * @brief Declare every data
 * @param mark1_ pointer to a system_t structure
 */
void DATA_LOGGER_Init(void)
{
	/* Retrieve structure from modules */
	float * gyro = MPU_Get_Gyro_Ptr();
	float * gyro_raw = MPU_Get_Gyro_Raw_Ptr();
	float * acc = MPU_Get_Acc_Ptr();
	float * acc_raw = MPU_Get_Acc_Raw_Ptr();
	float * angle = COMPLEMENTARY_FILTER_Get_Angles();

	/* -------------- Outputs ----------------- */
	DEFINE_DATA(data_id_eANGLE_ROLL,			(void*)&angle[axe_eROLL],			data_format_e16B_FLOAT_2D,		"Angle Roll",		use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eANGLE_PITCH,			(void*)&angle[axe_ePITCH],			data_format_e16B_FLOAT_2D,		"Angle Pitch",		use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eANGLE_YAW,				(void*)&angle[axe_eYAW],			data_format_e16B_FLOAT_2D,		"Angle Yaw",		use_format_eAS_OUTPUT);

	DEFINE_DATA(data_id_eGYRO_ROLL,				(void*)&gyro[axe_eROLL],			data_format_e16B_FLOAT_2D,		"Gyro Roll",		use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eGYRO_PITCH,			(void*)&gyro[axe_ePITCH],			data_format_e16B_FLOAT_2D,		"Gyro Pitch",		use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eGYRO_YAW,				(void*)&gyro[axe_eYAW],				data_format_e16B_FLOAT_2D,		"Gyro Yaw",			use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eGYRO_RAW_ROLL,			(void*)&gyro_raw[axe_eROLL],		data_format_e16B_FLOAT_2D,		"Gyro Roll Raw",	use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eGYRO_RAW_PITCH,		(void*)&gyro_raw[axe_ePITCH],		data_format_e16B_FLOAT_2D,		"Gyro Pitch Raw",	use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eGYRO_RAW_YAW,			(void*)&gyro_raw[axe_eYAW],			data_format_e16B_FLOAT_2D,		"Gyro Yaw Raw",		use_format_eAS_OUTPUT);

	DEFINE_DATA(data_id_eACC_ROLL,				(void*)&acc[axe_eROLL],				data_format_e16B_FLOAT_2D,		"Acc Roll",			use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eACC_PITCH,				(void*)&acc[axe_ePITCH],			data_format_e16B_FLOAT_2D,		"Acc Pitch",		use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eACC_YAW,				(void*)&acc[axe_eYAW],				data_format_e16B_FLOAT_2D,		"Acc Yaw",			use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eACC_RAW_ROLL,			(void*)&acc_raw[axe_eROLL],			data_format_e16B_FLOAT_2D,		"Acc Roll Raw",		use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eACC_RAW_PITCH,			(void*)&acc_raw[axe_ePITCH],		data_format_e16B_FLOAT_2D,		"Acc Pitch Raw",	use_format_eAS_OUTPUT);
	DEFINE_DATA(data_id_eACC_RAW_YAW,			(void*)&acc_raw[axe_eYAW],			data_format_e16B_FLOAT_2D,		"Acc Yaw Raw",		use_format_eAS_OUTPUT);

	/* -------------- Inputs ----------------- */
	DEFINE_DATA(data_id_eCONFIG_REQUEST,		NULL,	data_format_e0B_BUTTON,		"Send Configuration",	use_format_eAS_INPUT);
	DEFINE_DATA(data_id_eSTART_TRANSFER,		NULL,	data_format_e0B_BUTTON,		"Start Transfer",		use_format_eAS_INPUT);
	DEFINE_DATA(data_id_eSTOP_TRANSFER,			NULL,	data_format_e0B_BUTTON,		"Stop Transfer",		use_format_eAS_INPUT);
}
/*
 * @brief Request to start logging data
 */
void DATA_LOGGER_Start()
{
	start_flag = TRUE;
}
/*
 * @brief Request to stop logging data
 */
void DATA_LOGGER_Stop()
{
	stop_flag = TRUE;
}

/*
 * @brief Call the state machine and the uart parser
 */
void DATA_LOGGER_Main(void)
{
	state_machine();
	parse_uart();
}
/*
 * @brief Parse input data
 * @param input_buffer Buffer to use to parse the data
 */
void DATA_LOGGER_Reception(uint8_t * input_buffer)
{
	uint8_t id = input_buffer[0];
	uint16_t tmp_uint_16 = 0 ;
	int16_t tmp_int_16 = 0;
	switch(data_list[id].format)
	{
		case data_format_e0B_BUTTON:
			/* Link an action to each id */
			switch(id)
			{
				default:
					break;
				case data_id_eCONFIG_REQUEST:
					send_config_flag = TRUE;
					break;
				case data_id_eSTART_TRANSFER:
					DATA_LOGGER_Start();
					break;
				case data_id_eSTOP_TRANSFER:
					DATA_LOGGER_Stop();
					break;
			}
			break;
		case data_format_e8B:
			break;
		case data_format_e8B_FLOAT_0D:
			break;
		case data_format_e8B_FLOAT_1D:
			break;
		case data_format_e8B_PWM:
			tmp_uint_16 = 4*input_buffer[1];
			break;
		case data_format_e16B_INT16:
			tmp_int_16 = (int16_t)input_buffer[1] << 8;
			tmp_int_16 += (int16_t)input_buffer[2];
			switch(id)
			{
				default:
					break;
			}

			break;
		case data_format_e16B_UINT16:
			tmp_uint_16 = (uint16_t)input_buffer[1] << 8;
			tmp_uint_16 += (uint16_t)input_buffer[2];
			switch(id)
			{
				default:
					break;
			}

		case data_format_e16B_FLOAT_1D:
			break;
		default:
			break;

	}
}
/*
 * @brief Write a data id and his value into a buffer
 * @param id Id of the data
 * @param buffer Buffer array
 * @return Number of bytes written
 */
uint8_t DATA_LOGGER_Get_Data_Value(data_id_e id, uint8_t * buffer)
{

	/* Local temporary variables */
	int16_t tmp_16 = 0;
	float tmp_float = 0;

	/* First byte is reserved for the id */
	buffer[0] = id ;
	switch(data_list[id].format)
	{
		case data_format_e8B:
			buffer[1] = *(uint8_t*)data_list[id].data ;
			break;
		case data_format_e8B_PWM:
			break;
		case data_format_e8B_FLOAT_0D:
			tmp_float = *(float*)data_list[id].data ;
			buffer[1] = (int8_t)tmp_float;
			break;
		case data_format_e8B_FLOAT_1D:
			break;
		case data_format_e16B:
			break;
		case data_format_e16B_FLOAT_1D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 10;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case data_format_e16B_FLOAT_2D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 100;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case data_format_e16B_FLOAT_3D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 1000;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case data_format_e16B_UINT16:
			tmp_16 = *(uint16_t*)data_list[id].data ;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case data_format_e16B_INT16:
			tmp_16 = *(int16_t*)data_list[id].data ;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		default:
			break;
	}
	/* Length of the data array */
	return (data_list[id].format / 20) + 1;
}

/*
 * @brief Write a data configuration in a buffer
 * @param id Id of the data
 * @param buffer Buffer array to write the configuration into
 * @return Number of bytes we wrote in the buffer
 */
uint8_t DATA_LOGGER_Get_Data_Config(data_id_e id, uint8_t * buffer)
{
	/* We send a configuration so it must start with 0xFF */
	buffer[0] = 0xFF;
	/* Then we put the length of the name */
	buffer[1] = data_list[id].len_name;
	/* Then we write the data format */
	buffer[2] = data_list[id].format;
	/* Then the id of the data */
	buffer[3] = id;
	/* When the data is used as output, we add 128 to the id */
	if(data_list[id].use_format == use_format_eAS_INPUT)
	{
		buffer[3] += 128 ;
	}
	/* Finally we write the name */
	memcpy(&buffer[4], data_list[id].name, data_list[id].len_name);

	return 4 + data_list[id].len_name ;
}

/*
 * @brief Get data
 * @param id Id of the data
 * @return pointer to the data structure with the corresponding ID
 */
data_t * DATA_LOGGER_Get_Data(data_id_e id)
{
	return &data_list[id];
}


/* ----------------- Privates functions ----------------- */
/*
 * @brief Add the start byte and checksum to a message
 * @param buffer Data array to send
 * @param len Number of bytes to send
 */
void push_data_to_uart(uint8_t * buffer, uint8_t len)
{
	uint8_t buffer_new[80];
	uint32_t checksum = 0 ;
	buffer_new[0] = '$';
	for(uint8_t b = 0; b < len; b++)
	{
		buffer_new[1 + b] = buffer[b];
		checksum += (uint32_t)buffer_new[1 + b];
	}
	buffer_new[1 + len] = (uint8_t)(checksum % 256) ;
	UART_Transmit(UART_TELEMETRY, buffer_new, len+2);
}

/*
 * @brief data logger state machine
 */
static void state_machine(void)
{
	/* Time management for periodic transmissions */
	static uint32_t next_time_state_machine = 0;
	static uint32_t current_period = PERIODE_PING;
	uint32_t time_ms = HAL_GetTick();
	if(time_ms < next_time_state_machine)
	{
		return;
	}
	next_time_state_machine = time_ms + current_period;

	/* State machine variables */
	static data_logger_state_e state = data_logger_state_eIDLE;
	static data_logger_state_e previous_state = data_logger_state_eTRANSMIT_CONFIG ;
	bool_e entrance = previous_state != state ;
	previous_state = state ;
	switch(state)
	{
		case data_logger_state_eIDLE :
			if(entrance)
			{
				/* When we are in the idle mode, we just ping the device at PERIODE_PING µs interval */
				current_period = PERIODE_PING;
			}

			tmp_len = DATA_LOGGER_Get_Data_Config(data_id_eCONFIG_REQUEST, tmp);
			push_data_to_uart(tmp, tmp_len);

			/* If we receive a configuration request, we change the state */
			if(send_config_flag)
			{
				/* Reset flags */
				start_flag = FALSE;
				stop_flag = FALSE;
				send_config_flag = FALSE;
				/* Change the state */
				/* We send the configuration only if we did not send it before */
				if(config_not_sent)
				{
					id_init_counter = 0;
					is_any_data_used = FALSE;
					state = data_logger_state_eTRANSMIT_CONFIG;
				}
				else
				{
					state = data_logger_state_eLOG;
				}

			}
			/* Restart the transmission */
			else if(start_flag)
			{
				start_flag = FALSE;
				stop_flag = FALSE;
				state = data_logger_state_eLOG;
			}
			break;

		case data_logger_state_eTRANSMIT_CONFIG :
			/* Change the data logger task period to send configuration slowly */
			if(entrance)
			{
				current_period = PERIODE_CONFIG_SEND;
			}
			while(data_list[id_init_counter].use_format == use_format_eNOT_USED && id_init_counter < data_id_eCOUNT)
			{
				id_init_counter ++ ;
			}
			if(id_init_counter < data_id_eCOUNT)
			{
				is_any_data_used = TRUE;
				/* Get data configuration */
				tmp_len = DATA_LOGGER_Get_Data_Config(id_init_counter++, tmp);
				/* Send the configuration */
				push_data_to_uart(tmp, tmp_len);
			}
			else
			{
				if(is_any_data_used)
				{
					state = data_logger_state_eLOG ;
				}
				else
				{
					state = data_logger_state_eIDLE ;
				}
			}
			break;

		case data_logger_state_eLOG:
			if(entrance)
			{
				current_period = PERIODE_SEND;
			}
			if(stop_flag)
			{
				state = data_logger_state_eIDLE;
			}
			for(uint8_t d = 0; d < data_id_eCOUNT; d++)
			{
				if(data_list[d].use_format == use_format_eAS_OUTPUT && (data_list[d].data != NULL))
				{
					tmp_len = DATA_LOGGER_Get_Data_Value(d, tmp);
					push_data_to_uart(tmp, tmp_len);
				}
			}
			break;
	}
}

/*
 * @brief Parse incoming bytes
 */
static void parse_uart(void)
{
	/* state machine state variable */
	static parser_state_e state = parser_state_eSTART_BYTE;

	static uint8_t trame[20] = { 0 };
	static uint8_t nb_byte_to_buffer = 0;
	static uint8_t buffer_counter = 0;
	static uint16_t checksum = 0;
	/* Check for available data */
	if(UART_Availables(UART_TELEMETRY))
	{
		uint8_t c;
		UART_Get(UART_TELEMETRY, &c);
		switch(state)
		{
			case parser_state_eSTART_BYTE:
				if(c == '$')
				{
					state = parser_state_eID;
				}
				break;

			case parser_state_eID:
				if(c < data_id_eCOUNT)
				{
					/* Store the id for later use */
					trame[0] = c;
					/* Reset the checksum */
					checksum = c;
					/* Get the data length according to the data format */
					nb_byte_to_buffer = (DATA_LOGGER_Get_Data(c)->format / 20);
					if(nb_byte_to_buffer)
					{
						state = parser_state_eDATA;
					}
					else
					{
						state = parser_state_eCHECKSUM;
					}
				}
				else
				{
					state = parser_state_eSTART_BYTE;
				}
				break;
			case parser_state_eDATA:
				trame[1+buffer_counter++] = c;
				checksum += c;
				if(buffer_counter == nb_byte_to_buffer)
				{
					state = parser_state_eCHECKSUM;
				}
				break;
			case parser_state_eCHECKSUM:
				if(checksum == c)
				{
					DATA_LOGGER_Reception(trame);
				}
				state = parser_state_eSTART_BYTE;
				break;


		}
	}

}









