/*
 * events.c
 *
 *  Created on: 13 sept. 2020
 *      Author: Th�o
 */

#include "mask_def.h"
#include "flags.h"
#include "events.h"
#include "../scheduler/scheduler.h"
#include "../Flight_mode/Flight_mode.h"

//Ensemble des flags
volatile static Mask_t flags ;
volatile static bool_e new_flag = FALSE ;
static system_t * sys ;

//Si false, init du module event
static bool_e initialized = FALSE ;




#define DEFINE_EVENT(event_function_param, nb_mask_param, enable_param){  	\
		.function = event_function_param ,						\
		.nb_mask = nb_mask_param , 								\
		.state = enable_param									\
}


//	----------------------	Events --------------------------------------------------------------------------------
static void gyro_init_ok_func(mask_def_ids_t mask_id);
static void acc_init_ok_func(mask_def_ids_t mask_id);
static void ibus_data_rdy(mask_def_ids_t mask_id);
static void on_the_ground(mask_def_ids_t mask_id);
static void manual_accro(mask_def_ids_t mask_id);
static void gyro_data_ready_func(mask_def_ids_t mask_id);
static void acc_data_ready_func(mask_def_ids_t mask_id);
static void baro_data_ready_func(mask_def_ids_t mask_id);
static void orientation_update(mask_def_ids_t mask_id);
static void arming(mask_def_ids_t mask_id);
static void gyro_acc_calibration(mask_def_ids_t mask_id);
static void manual_angle(mask_def_ids_t mask_id);
static void manual_angle_altitude_auto(mask_def_ids_t mask_id);

//Attention !!!! nb_mask <= EVENT_NB_MASK_PER_EVENT_MAX else failure :)
static Event_t events_main[EVENT_COUNT] ={
		//Events array
		[EVENT_GYRO_INIT_OK] = 									DEFINE_EVENT(gyro_init_ok_func, 					MASK_GYRO_INIT_COUNT, 							EVENT_ENABLED),
		[EVENT_ACC_INIT_OK] = 									DEFINE_EVENT(acc_init_ok_func, 						MASK_ACC_INIT_COUNT, 							EVENT_DISABLED),
		[EVENT_GYRO_DATA_READY] = 								DEFINE_EVENT(gyro_data_ready_func, 					MASK_GYRO_DATA_READY_COUNT, 					EVENT_ENABLED),
		[EVENT_ACC_DATA_READY] = 								DEFINE_EVENT(acc_data_ready_func, 					MASK_ACC_DATA_READY_COUNT, 						EVENT_ENABLED),
		[EVENT_ORIENTATION_UPDATE] = 							DEFINE_EVENT(orientation_update, 					MASK_ORIENTATION_UPDATE_COUNT,					EVENT_ENABLED),
		[EVENT_IBUS_DATA_RDY] = 								DEFINE_EVENT(ibus_data_rdy, 						MASK_IBUS_DATA_RDY_COUNT, 						EVENT_ENABLED),
		[EVENT_BARO_DATA_RDY] = 								DEFINE_EVENT(baro_data_ready_func, 					MASK_BARO_DATA_READY_COUNT, 					EVENT_ENABLED),

		[EVENT_ON_THE_GROUND] = 								DEFINE_EVENT(on_the_ground, 						MASK_ON_THE_GROUND_COUNT, 						EVENT_ENABLED),
		[EVENT_MANUAL_ACCRO] = 									DEFINE_EVENT(manual_accro, 							MASK_MANUAL_ACCRO_COUNT, 						EVENT_ENABLED),
		[EVENT_MANUAL_ANGLE] = 									DEFINE_EVENT(manual_angle, 							MASK_MANUAL_ANGLE_COUNT, 						EVENT_ENABLED),
		[EVENT_MANUAL_ANGLE_ALTITUDE_AUTO] = 					DEFINE_EVENT(manual_angle_altitude_auto, 			MASK_MANUAL_ANGLE_ALTI_AUTO_COUNT, 				EVENT_ENABLED),
		[EVENT_ARMING] = 										DEFINE_EVENT(arming, 								MASK_ARMING_COUNT, 								EVENT_ENABLED),
		[EVENT_GYRO_ACC_CALIBRATION] = 							DEFINE_EVENT(gyro_acc_calibration, 					MASK_GYRO_ACC_COUNT, 							EVENT_ENABLED),



};

static void gyro_init_ok_func(mask_def_ids_t mask_id){
	//On dit que le gyro est ok et qu'il effectu des mesures
	EVENT_Set_flag(FLAG_GYRO_READING);
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_GYRO_UPDATE, TASK_MODE_TIME);
	events_main[EVENT_GYRO_INIT_OK].state = EVENT_DISABLED ;
}

static void acc_init_ok_func(mask_def_ids_t mask_id){
	//On dit que le gyro est ok et qu'il effectu des mesures
	EVENT_Set_flag(FLAG_ACC_READING);
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_ACC_UPDATE, TASK_MODE_TIME);
	events_main[EVENT_ACC_INIT_OK].state = EVENT_DISABLED ;
}

static void ibus_data_rdy(mask_def_ids_t mask_id){
	//On clean le flag ibus_data_rdy
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_IBUS_DATA_RDY);
	__enable_irq();
	//On déclenche la tâche de traitement des octets pour l ibus
	SCHEDULER_task_set_mode(TASK_CONTROLLER_CHANNEL_UPDATE, TASK_MODE_EVENT);
}

static void baro_data_ready_func(mask_def_ids_t mask_id){
	switch(mask_id){
		case MASK_BARO_DATA_READY_TEMP_RAW:
			EVENT_Clean_flag(FLAG_BARO_TEMP_RAW_RDY);
			SCHEDULER_task_set_mode(TASK_BARO_TEMP, TASK_MODE_EVENT);
			break;
		case MASK_BARO_DATA_READY_PRESSURE_RAW:
			EVENT_Clean_flag(FLAG_BARO_PRESSURE_RAW_RDY);
			SCHEDULER_task_set_mode(TASK_BARO_PRESSURE, TASK_MODE_EVENT);
			break;
		case MASK_BARO_DATA_READY_PRESSURE:
			EVENT_Clean_flag(FLAG_BARO_PRESSURE_RDY);
			SCHEDULER_task_set_mode(TASK_BARO_ALTITUDE, TASK_MODE_EVENT);
			break;
		default:
			break;

	}
}

static void on_the_ground(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_FLYING);
	MASK_clean_flag(&flags, FLAG_ARMED);
	MASK_clean_flag(&flags, FLAG_BUSY);
	MASK_clean_flag(&flags, FLAG_ARMING);
	MASK_clean_flag(&flags, FLAG_GYRO_CALI_IN_PROGRESS);
	__enable_irq();
	FLIGHT_MODE_Set_Flight_Mode(FLIGHT_MODE_ON_THE_GROUND);
}
static void manual_accro(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_set_flag(&flags, FLAG_FLYING);
	MASK_set_flag(&flags, FLAG_MANUAL);
	__enable_irq();
	FLIGHT_MODE_Set_Flight_Mode(FLIGHT_MODE_MANUAL_ACCRO);
}

static void manual_angle(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_set_flag(&flags, FLAG_FLYING);
	MASK_set_flag(&flags, FLAG_MANUAL);
	__enable_irq();
	FLIGHT_MODE_Set_Flight_Mode(FLIGHT_MODE_MANUAL_ANGLE);
}

static void manual_angle_altitude_auto(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_set_flag(&flags, FLAG_FLYING);
	MASK_set_flag(&flags, FLAG_MANUAL);
	__enable_irq();
	FLIGHT_MODE_Set_Flight_Mode(FLIGHT_MODE_MANUAL_ALTI_AUTO);
}

static void gyro_data_ready_func(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_GYRO_DATA_READY);
	__enable_irq();
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_GYRO_FILTER, TASK_MODE_EVENT);
}

static void acc_data_ready_func(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_ACC_DATA_READY);
	__enable_irq();
	//On lance la tâche d'update du gyro
	SCHEDULER_task_set_mode(TASK_ACC_FILTER, TASK_MODE_EVENT);
}

static void orientation_update(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_clean_flag(&flags, FLAG_ACC_FILTERED_DATA_READY);
	MASK_clean_flag(&flags, FLAG_GYRO_FILTERED_DATA_READY);
	__enable_irq();
	SCHEDULER_task_set_mode(TASK_ORIENTATION_UPDATE, TASK_MODE_EVENT);
}

static void arming(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_set_flag(&flags, FLAG_ARMING);
	MASK_set_flag(&flags, FLAG_BUSY);	//We re now busy => cant go in calibration mode or other "on the ground" modes
	__enable_irq();
	FLIGHT_MODE_Set_Flight_Mode(FLIGHT_MODE_ARMING);
}

static void gyro_acc_calibration(mask_def_ids_t mask_id){
	__disable_irq();
	MASK_set_flag(&flags, FLAG_GYRO_CALI_IN_PROGRESS);
	MASK_set_flag(&flags, FLAG_BUSY);
	MASK_clean_flag(&flags, FLAG_CHAN_9_PUSH);
	MASK_clean_flag(&flags, FLAG_GYRO_CALI_DONE);
	__enable_irq();
	FLIGHT_MODE_Set_Flight_Mode(FLIGHT_MODE_GYRO_ACC_CALIBRATION);
}


void EVENT_init(system_t * sys_){
	initialized = TRUE ;
	sys =sys_;

	//Configuration des mask associés aux events
	mask_def_events_init(events_main);
}

//Déclenchement des events en main
void EVENT_process(bool_e test_all){
	if(new_flag)
	{
		new_flag = FALSE;

		//Nombre d'event à tester
		uint8_t max = EVENT_LOW_LVL_COUNT ;
		if(test_all)
			max = EVENT_COUNT ;



		//Pout chaque event
		for(uint32_t e = 0; e < max; e ++)
		{

			uint32_t m = 0 ;
			bool_e function_did_run_once = FALSE ;

			//On test chaque paires de masque
			while(m < events_main[e].nb_mask && !function_did_run_once && events_main[e].state == EVENT_ENABLED)
			{
				if(Mask_test_and(events_main[e].mask_and[m], flags))			//Mask and test
				{
					if(Mask_test_or(events_main[e].mask_or[m], flags))			//Mask or test
					{
						if(!Mask_test_or(events_main[e].mask_not[m], flags))	//Mask not test
						{
							events_main[e].function(m);
							function_did_run_once = TRUE ;
						}
					}
				}
				m++ ;
			}

		}
	}
}

bool_e EVENT_Check_flag(Flags_e flag){
	return Mask_test_and(MASK_create_single(flag), flags);
}


//Set a flag
void EVENT_Set_flag(Flags_e flag){
	__disable_irq();					//It désactivitées pour éviter la réentrance
	new_flag = TRUE ;
	MASK_set_flag(&flags, flag);
	__enable_irq();
}
//CleaR a flag (i know it should be called cleaR instead of cleaN but i didnt took the time to change it YET)
//Todo : cleaRRRRRRR a flag
void EVENT_Clean_flag(Flags_e flag){
	__disable_irq();					//It désactivitées pour éviter la réentrance
	new_flag = TRUE ;
	MASK_clean_flag(&flags, flag);
	__enable_irq();
}








