/*
 * Data_logger.c
 *
 *  Created on: 22 déc. 2020
 *      Author: theom
 */

#include "Data_logger.h"
#include "string.h"
#include "../OS/tasks/task.h"
#include "../OS/scheduler/scheduler.h"
static system_t * sys ;





#define DEFINE_DATA(id_, ptr_, format_, name_, use_) {	\
	data_list[id_].id = id_ ;			\
	data_list[id_].data = ptr_ ;		\
	data_list[id_].format = format_;	\
	data_list[id_].name = name_;			\
	data_list[id_].len_name = sizeof(name_)-1;	\
	data_list[id_].used = use_;				\
}


typedef enum data_logger_state_e{
	LOGGER_IDDLE,
	LOGGER_TRANSMIT_CONFIG,
	LOGGER_LOG
}data_logger_state_e;


static data_t data_list[DATA_ID_COUNT] ;
static data_logger_state_e state = LOGGER_IDDLE;
static bool_e config_not_sent = TRUE ;
static bool_e start_flag = FALSE;
static bool_e send_config_flag = FALSE ;
static bool_e stop_flag = FALSE;
static uint8_t tmp[20];
static uint8_t tmp_len ;
static uint8_t id_init_counter = 0;
static bool_e is_any_data_used = FALSE;

//Data name def
static uint8_t name_roll_angle[] = "Angle ROLL";
static uint8_t name_pitch_angle[] = "Angle PITCH";

static uint8_t name_roll_angle_acc[] = "AnglAcc ROLL";
static uint8_t name_pitch_angle_acc[] = "AnglACC PITCH";

static uint8_t name_consigne_angle_roll[] = "Target ROLL";
static uint8_t name_consigne_angle_pitch[] = "Target PITCH";

static uint8_t name_roll_gyro [] = "Gyro ROLL";
static uint8_t name_pitch_gyro[] = "Gyro PITCH";
static uint8_t name_yaw_gyro[] = "Gyro YAW";

static uint8_t name_roll_acc[] = "Acc ROLL";
static uint8_t name_pitch_acc[] = "Acc PITCH";
static uint8_t name_yaw_acc[] = "Acc YAW";

static uint8_t name_roll_acc_raw[] = "Acc Raw ROLL";
static uint8_t name_pitch_acc_raw[] = "Acc Raw PITCH";
static uint8_t name_yaw_acc_raw[] = "Acc Raw YAW";


static uint8_t name_x_acc[] = "Acc X";
static uint8_t name_y_acc[] = "Acc Y";
static uint8_t name_z_acc[] = "Acc Z";

static uint8_t name_x_speed[] = "Speed X";
static uint8_t name_y_speed[] = "Speed Y";
static uint8_t name_z_speed[] = "Speed Z";

static uint8_t name_x_pos[] = "Pos X";
static uint8_t name_y_pos[] = "Pos Y";
static uint8_t name_z_pos[] = "Pos Z";

static uint8_t name_consigne_pos_z[] = "Target Pos Z";
static uint8_t name_consigne_velocity_z[] = "Target Vel Z";

static uint8_t name_pid_pos_z[] = "PID Pos Z";
static uint8_t name_p_pos_z[] = "P Pos Z";
static uint8_t name_i_pos_z[] = "I Pos Z";
static uint8_t name_d_pos_z[] = "D Pos Z";

static uint8_t name_pid_velocity_z[] = "PID Velo Z";
static uint8_t name_p_velocity_z[] = "P Velo Z";
static uint8_t name_i_velocity_z[] = "I Velo Z";
static uint8_t name_d_velocity_z[] = "D Velo Z";

static uint8_t name_target_roll_gyro[] = "Tar Gyro ROLL";
static uint8_t name_target_pitch_gyro[] = "Tar Gyro PITCH";
static uint8_t name_target_yaw_gyro[] = "Tar Gyro YAW";


static uint8_t name_roll_gyro_raw [] = "Gyro raw ROLL";
static uint8_t name_pitch_gyro_raw[] = "Gyro raw PITCH";
static uint8_t name_yaw_gyro_raw[] = "Gyro raw YAW";

static uint8_t name_pid_gyro_roll[] = "PID Gyro Roll";
static uint8_t name_pid_gyro_roll_p[] = "PID Gyro Ro P";
static uint8_t name_pid_gyro_roll_i[] = "PID Gyro Ro i";
static uint8_t name_pid_gyro_roll_d[] = "PID Gyro Ro D";

static uint8_t name_pid_gyro_pitch[] = "PID Gyro Pitch";
static uint8_t name_pid_gyro_yaw[] = "PID Gyro Yaw";

static uint8_t name_prop_thrust[] = "PROP Thrust";
static uint8_t name_moteur_fl[] = "Moteur FL";
static uint8_t name_moteur_fr[] = "Moteur FR";
static uint8_t name_moteur_br[] = "Moteur BR";
static uint8_t name_moteur_bl[] = "Moteur BL";

static uint8_t name_temperature[] = "Temperature";
static uint8_t name_pressure_raw[] = "Pressure Raw";
static uint8_t name_pressure[] = "Pressure";
static uint8_t name_altitude[] = "Altitude";
static uint8_t name_altitude_raw[] = "Altitude Raw";

static uint8_t name_config_request[] = "Send Config";
static uint8_t name_enable_asser_orientation[] = "En Asser Ori";
static uint8_t name_disable_asser_orientation[] = "Dis Asser Ori";
static uint8_t name_start_transfer[] = "Start Transfer";
static uint8_t name_stop_transfer[] = "Stop Transfer";



static uint8_t name_flight_mode[] = "Flight Mode";

void DATA_LOGGER_Init(system_t * sys_){

	sys = sys_;

	//	-----------------------------------------------	Définitions des data	-----------------------------------------------------------------
	//Angle
	DEFINE_DATA(DATA_ID_ROLL_ANGLE, (uint8_t*)&sys->orientation.angular_position[ORIENTATION_ROLL], 							DATA_FORMAT_16B_FLOAT_1D, 	name_roll_angle,							FALSE);
	DEFINE_DATA(DATA_ID_PITCH_ANGLE, (uint8_t*)&sys->orientation.angular_position[ORIENTATION_PITCH], 							DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_angle,							FALSE);

	//Angles acc
	DEFINE_DATA(DATA_ID_ROLL_ACC_ANGLE, (uint8_t*)&sys->orientation.acc_angles[ORIENTATION_ROLL], 								DATA_FORMAT_16B_FLOAT_1D, 	name_roll_angle_acc,						FALSE);
	DEFINE_DATA(DATA_ID_PITCH_ACC_ANGLE, (uint8_t*)&sys->orientation.acc_angles[ORIENTATION_PITCH], 							DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_angle_acc,						FALSE);

	//Consignes angles
	DEFINE_DATA(DATA_ID_CONSIGNE_ANGLE_ROLL, (uint8_t*)&sys->regulation.orientation.consigne_angular_pos[ORIENTATION_ROLL], 	DATA_FORMAT_16B_FLOAT_1D, 	name_consigne_angle_roll,					FALSE);
	DEFINE_DATA(DATA_ID_CONSIGNE_ANGLE_PITCH, (uint8_t*)&sys->regulation.orientation.consigne_angular_pos[ORIENTATION_PITCH], 	DATA_FORMAT_16B_FLOAT_1D, 	name_consigne_angle_pitch,					FALSE);

	//Pid gyro roll
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].output, 		DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL_P, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].P, 			DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll_p, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL_I, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].I, 			DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll_i, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_GYRO_ROLL_D, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_ROLL].D, 			DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_roll_d, 						FALSE);

	// Autres pid gyro
	DEFINE_DATA(DATA_ID_PID_GYRO_PITCH, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_PITCH].output, 	DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_pitch, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_GYRO_YAW, (uint8_t*)&sys->regulation.orientation.pid_angular_speed[ORIENTATION_YAW].output, 		DATA_FORMAT_16B_FLOAT_1D, 	name_pid_gyro_yaw, 							FALSE);

	//Angle rate
	DEFINE_DATA(DATA_ID_ROLL_GYRO, (uint8_t*)&sys->sensors.gyro.filtered[ORIENTATION_ROLL], 									DATA_FORMAT_16B_FLOAT_1D, 	name_roll_gyro, 							FALSE);
	DEFINE_DATA(DATA_ID_PITCH_GYRO, (uint8_t*)&sys->sensors.gyro.filtered[ORIENTATION_PITCH], 									DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_gyro, 							FALSE);
	DEFINE_DATA(DATA_ID_YAW_GYRO, (uint8_t*)&sys->sensors.gyro.filtered[ORIENTATION_YAW], 										DATA_FORMAT_16B_FLOAT_1D, 	name_yaw_gyro, 			 					FALSE);

	//Acceleration
	DEFINE_DATA(DATA_ID_ROLL_ACC, (uint8_t*)&sys->sensors.acc.filtered[ORIENTATION_ROLL], 										DATA_FORMAT_16B_FLOAT_3D, 	name_roll_acc, 								FALSE);
	DEFINE_DATA(DATA_ID_PITCH_ACC, (uint8_t*)&sys->sensors.acc.filtered[ORIENTATION_PITCH], 									DATA_FORMAT_16B_FLOAT_3D, 	name_pitch_acc, 							FALSE);
	DEFINE_DATA(DATA_ID_YAW_ACC, (uint8_t*)&sys->sensors.acc.filtered[ORIENTATION_YAW], 										DATA_FORMAT_16B_FLOAT_3D, 	name_yaw_acc, 								FALSE);

	//Acceleration
	DEFINE_DATA(DATA_ID_ROLL_ACC_RAW, (uint8_t*)&sys->sensors.acc.raw[ORIENTATION_ROLL], 										DATA_FORMAT_16B_FLOAT_3D, 	name_roll_acc_raw, 							FALSE);
	DEFINE_DATA(DATA_ID_PITCH_ACC_RAW, (uint8_t*)&sys->sensors.acc.raw[ORIENTATION_PITCH], 										DATA_FORMAT_16B_FLOAT_3D, 	name_pitch_acc_raw, 						FALSE);
	DEFINE_DATA(DATA_ID_YAW_ACC_RAW, (uint8_t*)&sys->sensors.acc.raw[ORIENTATION_YAW], 											DATA_FORMAT_16B_FLOAT_3D, 	name_yaw_acc_raw, 							FALSE);

	//Acceleration dans le ref de la terre
	DEFINE_DATA(DATA_ID_ACC_X, (uint8_t*)&sys->position.acceleration[POSITION_AXE_X], 											DATA_FORMAT_16B_FLOAT_3D, 	name_x_acc, 								FALSE);
	DEFINE_DATA(DATA_ID_ACC_Y, (uint8_t*)&sys->position.acceleration[POSITION_AXE_Y], 											DATA_FORMAT_16B_FLOAT_3D, 	name_y_acc, 								FALSE);
	DEFINE_DATA(DATA_ID_ACC_Z, (uint8_t*)&sys->position.acc_raw[POSITION_AXE_Z], 												DATA_FORMAT_16B_FLOAT_3D, 	name_z_acc, 								TRUE);

	//Vitesse dans le ref de la terre
	DEFINE_DATA(DATA_ID_SPEED_X, (uint8_t*)&sys->position.velocity[POSITION_AXE_X], 											DATA_FORMAT_16B_FLOAT_2D, 	name_x_speed, 								FALSE);
	DEFINE_DATA(DATA_ID_SPEED_Y, (uint8_t*)&sys->position.velocity[POSITION_AXE_Y], 											DATA_FORMAT_16B_FLOAT_2D, 	name_y_speed, 								FALSE);
	DEFINE_DATA(DATA_ID_SPEED_Z, (uint8_t*)&sys->position.velocity[POSITION_AXE_Z], 											DATA_FORMAT_16B_FLOAT_2D, 	name_z_speed, 								TRUE);

	//Position dans le ref de la terre
	DEFINE_DATA(DATA_ID_POS_X, (uint8_t*)&sys->position.position[POSITION_AXE_X], 												DATA_FORMAT_16B_FLOAT_3D, 	name_x_pos, 								FALSE);
	DEFINE_DATA(DATA_ID_POS_Y, (uint8_t*)&sys->position.position[POSITION_AXE_Y], 												DATA_FORMAT_16B_FLOAT_3D, 	name_y_pos, 								FALSE);
	DEFINE_DATA(DATA_ID_POS_Z, (uint8_t*)&sys->position.position[POSITION_AXE_Z], 												DATA_FORMAT_16B_FLOAT_3D, 	name_z_pos, 								FALSE);

	//Consigne position
	DEFINE_DATA(DATA_ID_CONSIGNE_POS_Z, (uint8_t*)&sys->regulation.position.consigne_position[POSITION_AXE_Z], 					DATA_FORMAT_16B_FLOAT_3D, 	name_consigne_pos_z, 						FALSE);


	//Pid position
	DEFINE_DATA(DATA_ID_PID_POS_Z, (uint8_t*)&sys->regulation.position.pid_position[POSITION_AXE_Z].output, 					DATA_FORMAT_16B_FLOAT_1D, 	name_pid_pos_z, 							FALSE);
	DEFINE_DATA(DATA_ID_PID_POS_Z_P, (uint8_t*)&sys->regulation.position.pid_position[POSITION_AXE_Z].P, 						DATA_FORMAT_16B_FLOAT_1D, 	name_p_pos_z, 								FALSE);
	DEFINE_DATA(DATA_ID_PID_POS_Z_I, (uint8_t*)&sys->regulation.position.pid_position[POSITION_AXE_Z].I, 						DATA_FORMAT_16B_FLOAT_1D, 	name_i_pos_z, 								FALSE);
	DEFINE_DATA(DATA_ID_PID_POS_Z_D, (uint8_t*)&sys->regulation.position.pid_position[POSITION_AXE_Z].D, 						DATA_FORMAT_16B_FLOAT_1D, 	name_d_pos_z, 								FALSE);

	//Consigne velocity
	DEFINE_DATA(DATA_ID_CONSIGNE_VELOCITY_Z, (uint8_t*)&sys->regulation.position.consigne_velocity[POSITION_AXE_Z], 			DATA_FORMAT_16B_FLOAT_1D, 	name_consigne_velocity_z, 					FALSE);

	//Pid velocity
	DEFINE_DATA(DATA_ID_PID_VELOCITY_Z, (uint8_t*)&sys->regulation.position.pid_velocity[POSITION_AXE_Z].output, 				DATA_FORMAT_16B_FLOAT_1D, 	name_pid_velocity_z, 					FALSE);
	DEFINE_DATA(DATA_ID_PID_VELOCITY_Z_P, (uint8_t*)&sys->regulation.position.pid_velocity[POSITION_AXE_Z].P, 					DATA_FORMAT_16B_FLOAT_1D, 	name_p_velocity_z, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_VELOCITY_Z_I, (uint8_t*)&sys->regulation.position.pid_velocity[POSITION_AXE_Z].I, 					DATA_FORMAT_16B_FLOAT_1D, 	name_i_velocity_z, 						FALSE);
	DEFINE_DATA(DATA_ID_PID_VELOCITY_Z_D, (uint8_t*)&sys->regulation.position.pid_velocity[POSITION_AXE_Z].D, 					DATA_FORMAT_16B_FLOAT_1D, 	name_d_velocity_z, 						FALSE);

	//Consignes angles rates
	DEFINE_DATA(DATA_ID_CONSIGNE_GYRO_ROLL, (uint8_t*)&sys->regulation.orientation.consigne_angular_speed[ORIENTATION_ROLL], 	DATA_FORMAT_16B_FLOAT_1D, 	name_target_roll_gyro, 							FALSE);
	DEFINE_DATA(DATA_ID_CONSIGNE_GYRO_PITCH, (uint8_t*)&sys->regulation.orientation.consigne_angular_speed[ORIENTATION_PITCH], 	DATA_FORMAT_16B_FLOAT_1D, 	name_target_pitch_gyro, 						FALSE);
	DEFINE_DATA(DATA_ID_CONSIGNE_GYRO_YAW, (uint8_t*)&sys->regulation.orientation.consigne_angular_speed[ORIENTATION_YAW], 		DATA_FORMAT_16B_FLOAT_1D, 	name_target_yaw_gyro, 		 					FALSE);

	//Angle Rate raw
	DEFINE_DATA(DATA_ID_ROLL_GYRO_RAW, (uint8_t*)&sys->sensors.gyro.raw[ORIENTATION_ROLL], 										DATA_FORMAT_16B_FLOAT_1D, 	name_roll_gyro_raw, 							FALSE);
	DEFINE_DATA(DATA_ID_PITCH_GYRO_RAW, (uint8_t*)&sys->sensors.gyro.raw[ORIENTATION_ROLL], 									DATA_FORMAT_16B_FLOAT_1D, 	name_pitch_gyro_raw,							FALSE);
	DEFINE_DATA(DATA_ID_YAW_GYRO_RAW, (uint8_t*)&sys->sensors.gyro.raw[ORIENTATION_YAW], 										DATA_FORMAT_16B_FLOAT_1D, 	name_yaw_gyro_raw, 								FALSE);

	//Barometer
	DEFINE_DATA(DATA_ID_PRESSURE_RAW, 		(uint8_t*)sys->sensors.baro.pressure_raw, 											DATA_FORMAT_16B_UINT16, 	name_pressure_raw, 								FALSE);
	DEFINE_DATA(DATA_ID_PRESSURE, 		(uint8_t*)&sys->sensors.baro.pressure, 													DATA_FORMAT_16B_UINT16, 	name_pressure, 									FALSE);
	DEFINE_DATA(DATA_ID_TEMPERATURE, 	(uint8_t*)&sys->sensors.ms5611.temperature, 											DATA_FORMAT_16B_INT16, 		name_temperature, 								FALSE);
	DEFINE_DATA(DATA_ID_ALTITUDE, 		(uint8_t*)&sys->sensors.baro.altitude, 													DATA_FORMAT_16B_FLOAT_3D, 	name_altitude, 									TRUE);
	DEFINE_DATA(DATA_ID_ALTITUDE_RAW, 		(uint8_t*)&sys->sensors.baro.altitude_raw, 											DATA_FORMAT_16B_FLOAT_3D, 	name_altitude_raw, 								FALSE);

	//Buttons
	DEFINE_DATA(DATA_ID_CONFIG_REQUEST, NULL, 																					DATA_FORMAT_0B_BUTTON, 		name_config_request, 							FALSE);
	DEFINE_DATA(DATA_ID_DISABLE_ASSER_ORIENTATION, NULL, 																		DATA_FORMAT_0B_BUTTON, 		name_disable_asser_orientation,  				TRUE);
	DEFINE_DATA(DATA_ID_ENABLE_ASSER_ORIENTATION, NULL, 																		DATA_FORMAT_0B_BUTTON, 		name_enable_asser_orientation, 					TRUE);
	DEFINE_DATA(DATA_ID_START_TRANSFER, NULL, 																					DATA_FORMAT_0B_BUTTON, 		name_start_transfer, 		 					TRUE);
	DEFINE_DATA(DATA_ID_STOP_TRANSFER, NULL, 																					DATA_FORMAT_0B_BUTTON, 		name_stop_transfer, 							TRUE);

	//Propulsion
	DEFINE_DATA(DATA_ID_PROP_THRUST, (uint8_t*)&sys->propulsion.consigne[PROP_CONSIGNE_THRUST], 								DATA_FORMAT_16B_UINT16,  	name_prop_thrust, 								TRUE);
	DEFINE_DATA(DATA_ID_MOTOR_FL, 	(uint8_t*)&sys->propulsion.motors_outputs[MOTOR_FL], 										DATA_FORMAT_16B_UINT16,  	name_moteur_fl, 								FALSE);
	DEFINE_DATA(DATA_ID_MOTOR_FR, 	(uint8_t*)&sys->propulsion.motors_outputs[MOTOR_FR], 										DATA_FORMAT_16B_UINT16,  	name_moteur_fr, 								FALSE);
	DEFINE_DATA(DATA_ID_MOTOR_BR, 	(uint8_t*)&sys->propulsion.motors_outputs[MOTOR_BR], 										DATA_FORMAT_16B_UINT16,  	name_moteur_br, 								FALSE);
	DEFINE_DATA(DATA_ID_MOTOR_BL, 	(uint8_t*)&sys->propulsion.motors_outputs[MOTOR_BL], 										DATA_FORMAT_16B_UINT16,  	name_moteur_bl, 								FALSE);




	//Others
	DEFINE_DATA(DATA_ID_FLIGHT_MODE, sys->soft.flight_mode, 																	DATA_FORMAT_8B, 			name_flight_mode, 								FALSE);


}

void DATA_LOGGER_Start(){
	start_flag = TRUE;
}

void DATA_LOGGER_Stop(){
	stop_flag = TRUE;
}

void DATA_LOGGER_Main(void){

	static data_logger_state_e previous_state 	= LOGGER_TRANSMIT_CONFIG ;
	bool_e entrance = previous_state != state ;
	previous_state = state ;

	switch(state){
		case LOGGER_IDDLE :
			if(entrance){
				//Quand on passe en mode iddle en envoit l'id du boutton de config toute les 2 secondes
				SCHEDULER_reschedule_task(TASK_LOGGER, 2000000);
			}

			tmp_len = DATA_LOGGER_Get_Data_Config(DATA_ID_CONFIG_REQUEST, tmp);
			TELEMETRY_Send_Data(tmp, tmp_len);

			//Si on a reçu une requête pour envoyer la config, on y go
			if(send_config_flag){
				//Reset flags
				start_flag = FALSE;
				stop_flag = FALSE;
				send_config_flag = FALSE;
				//Change the state
				if(config_not_sent){
					id_init_counter = 0;
					is_any_data_used = FALSE;
					state = LOGGER_TRANSMIT_CONFIG;

				}
				else
					state = LOGGER_LOG;
			}
			else if(start_flag)
			{
				start_flag = FALSE;
				stop_flag = FALSE;
				state = LOGGER_LOG;
			}
			break;
		case LOGGER_TRANSMIT_CONFIG :

			if(entrance)
				SCHEDULER_reschedule_task(TASK_LOGGER, 500000);	//On abaisse la fréquence de la tâche pendant l'init

			//If the data[id_init_counter] is used
			while(!data_list[id_init_counter].used && id_init_counter < DATA_ID_COUNT){
				id_init_counter ++ ;
			}
			if(id_init_counter < DATA_ID_COUNT){
				is_any_data_used = TRUE;
				//Get a data config
				tmp_len = DATA_LOGGER_Get_Data_Config(id_init_counter++, tmp);
				//Send the data config
				TELEMETRY_Send_Data(tmp, tmp_len);
			}

			else
			{
				if(is_any_data_used)
				{

					config_not_sent = FALSE ;
					state = LOGGER_LOG ;
				}
				else
					state = LOGGER_IDDLE ;

			}

			break;

		case LOGGER_LOG:
			if(entrance)
				SCHEDULER_reschedule_task(TASK_LOGGER, 10000);	//On reprend une fréquence plus importante pour l'envoit en flux continu

			if(stop_flag)
				state = LOGGER_IDDLE;

			for(uint8_t d = 0; d < DATA_ID_COUNT; d++)
			{
				if(data_list[d].used && (data_list[d].format != DATA_FORMAT_0B_BUTTON))
				{
					tmp_len = DATA_LOGGER_Get_Data_Value(d, tmp);
					TELEMETRY_Send_Data(tmp, tmp_len);
				}
			}
			break;
	}

}

void DATA_LOGGER_Reception(uint8_t * input_buffer){
	uint8_t id = input_buffer[0];

	switch(data_list[id].format){
		case DATA_FORMAT_0B_BUTTON:
			//We can create an action for each id
			switch(id)
			{
				default:
					break;
				case DATA_ID_CONFIG_REQUEST:
					send_config_flag = TRUE;
					break;
				case DATA_ID_ENABLE_ASSER_ORIENTATION:
					REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_ACCRO);
					break;
				case DATA_ID_DISABLE_ASSER_ORIENTATION:
					REGULATION_ORIENTATION_Set_Regulation_Mode(REGULATION_ORIENTATION_MODE_OFF);
					break;
				case DATA_ID_START_TRANSFER:
					DATA_LOGGER_Start();
					break;
				case DATA_ID_STOP_TRANSFER:
					DATA_LOGGER_Stop();
					break;
			}
			break;
		case DATA_FORMAT_8B:
			break;
		case DATA_FORMAT_8B_FLOAT_0D:
			break;
		case DATA_FORMAT_8B_FLOAT_1D:
			break;
		case DATA_FORMAT_8B_PWM:
			break;
		case DATA_FORMAT_16B:
			break;
		case DATA_FORMAT_16B_FLOAT_1D:
			break;
		default:
			break;

	}
}

uint8_t DATA_LOGGER_Get_Data_Value(data_id_e id, uint8_t * buffer){

//	uint16_t tmp_u16 = 0;
	int16_t tmp_16 = 0;
	float tmp_float = 0;

	//Premier octet contient l'id
	buffer[0] = id ;

	//Formatage de la donnée en fonction du type et du format
	switch(data_list[id].format){
		case DATA_FORMAT_8B:
			buffer[1] = *data_list[id].data ;
			break;
		case DATA_FORMAT_8B_PWM:
			break;
		case DATA_FORMAT_8B_FLOAT_0D:
			tmp_float = *(float*)data_list[id].data ;
			buffer[1] = (int8_t)tmp_float;
			break;
		case DATA_FORMAT_8B_FLOAT_1D:
			break;
		case DATA_FORMAT_16B:
			break;
		case DATA_FORMAT_16B_FLOAT_1D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 10;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case DATA_FORMAT_16B_FLOAT_2D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 100;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case DATA_FORMAT_16B_FLOAT_3D:
			tmp_float = *(float*)data_list[id].data;
			tmp_16 = tmp_float * 1000;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case DATA_FORMAT_16B_UINT16:
			tmp_16 = *(uint16_t*)data_list[id].data ;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		case DATA_FORMAT_16B_INT16:
			tmp_16 = *(int16_t*)data_list[id].data ;
			buffer[1] = (uint8_t)((tmp_16 >> 8) & 0b11111111) ;
			buffer[2] = (uint8_t)(tmp_16 & 0b11111111);
			break;
		default:
			break;
	}

	//Return the len of data
	return (data_list[id].format / 20) + 1;

}

uint8_t DATA_LOGGER_Get_Data_Config(data_id_e id, uint8_t * buffer){
	//New data so we start with 255
	buffer[0] = 255;
	//Then name len
	buffer[1] = data_list[id].len_name;
	//Then format
	buffer[2] = data_list[id].format ;
	//Taille du nom
	buffer[3] = id	 ;
	//Copie du nom
	memcpy(&buffer[4], data_list[id].name, data_list[id].len_name);

	//And that s it
	return 4 + data_list[id].len_name ;
}

data_t * DATA_LOGGER_Get_Data(data_id_e id){
	return &data_list[id];
}



