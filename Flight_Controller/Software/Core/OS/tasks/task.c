/*
 * task.c
 *
 *  Created on: 13 aout 2020
 *      Author: Theo
 */

#include "task.h"
#include "../scheduler/scheduler.h"
#include "../events/events.h"
#include "../Flight_mode/Flight_mode.h"
#include "../Data_Logger/Data_logger.h"
#include "../OS/time.h"


static system_t * sys;

static uint32_t start_time_gyro = 0;
static uint32_t end_time_filtering_gyro= 0;


void tasks_init(system_t * sys_){
	//On mémorise où se trouve notre structure système
	sys = sys_ ;

	//Activation des tâches
	SCHEDULER_enable_task(TASK_SCHEDULER, TRUE);
	SCHEDULER_enable_task(TASK_EVENT_CHECK, TRUE);

	SCHEDULER_enable_task(TASK_LED, TRUE);
	//SCHEDULER_enable_task(TASK_PRINTF, TRUE);
	SCHEDULER_enable_task(TASK_ORIENTATION_UPDATE, TRUE);
	SCHEDULER_enable_task(TASK_GYRO_FILTER, TRUE);
	SCHEDULER_enable_task(TASK_GYRO_UPDATE, TRUE);
	SCHEDULER_enable_task(TASK_ACC_FILTER, TRUE);
	SCHEDULER_enable_task(TASK_CONTROLLER_CHANNEL_UPDATE, TRUE);
	SCHEDULER_enable_task(TASK_CONTROLLER_CHANNEL_ANALYSIS, TRUE);
	SCHEDULER_enable_task(TASK_HIGH_LVL, TRUE);
	SCHEDULER_enable_task(TASK_TELEMETRIE, TRUE);
	SCHEDULER_enable_task(TASK_LOGGER, TRUE);
	SCHEDULER_enable_task(TASK_SELF_TEST, TRUE);
	SCHEDULER_enable_task(TASK_BARO_UPDATE, TRUE);
	SCHEDULER_enable_task(TASK_BARO_TEMP, TRUE);
	SCHEDULER_enable_task(TASK_BARO_PRESSURE, TRUE);
	SCHEDULER_enable_task(TASK_BARO_ALTITUDE, TRUE);

}

void process_print_f(uint32_t current_time_us){
	//printf("%d\t%d\t%d\t%d\t%d\t%d\n", data[0], data[1], data[2], data[3], data[4], data[5]);
//	gyro_t * gyro = &sys->sensors.gyro ;
//	acc_t * acc = &sys->sensors.acc ;
	orientation_t * ori = &sys->orientation ;
	//printf("%d\t%d\t%d\t%lu\n",sys->sensors.gyro.mpu->gyro_raw[0], sys->sensors.gyro.mpu->gyro_raw[1], sys->sensors.gyro.mpu->gyro_raw[2], TASK_get_task(TASK_GYRO)->it_duration_us);
	//printf("%f\t%f\t%f\t%f\t%f\t%f\t%lu\n",acc->raw[0], acc->raw[1], acc->raw[2],gyro->raw[0], gyro->raw[1], gyro->raw[2], TASK_get_task(TASK_GYRO_UPDATE)->duration_us);
	//printf("%f\t%f\t%lu\n", acc->filtered[ACC_AXE_Z], gyro->filtered[ACC_AXE_X], delta_1);
	//printf("%f\n", acc->filtered[GYRO_AXE_X]);
	//printf("%lu\n", delta_1);
	printf("%f\t%f\t%f\n", ori->angular_position[0], ori->angular_velocity[0], ori->acc_angles[0]);
}


void process_led(uint32_t current_time_us){
	IHM_Update();
	BUZZER_Update(&sys->ihm.buzzer);
}

void process_gyro_update(uint32_t current_time_us){
	start_time_gyro = current_time_us ;
	GYRO_ACC_update_dma(&sys->sensors.gyro);

}

void process_gyro_filter(uint32_t current_time_us){
	end_time_filtering_gyro = current_time_us ;

	GYRO_process_lpf(&sys->sensors.gyro);

}

void process_acc_update(uint32_t current_time_us){
	ACC_update(&sys->sensors.acc);
}

void process_acc_filter(uint32_t current_time_us){
	ACC_process_lpf(&sys->sensors.acc);
}

void process_event_main(uint32_t current_time_us){
	UNUSED(current_time_us);
	EVENT_process(FALSE);
}

void process_orientation_update(uint32_t current_time_us){

	//Estimation
	ORIENTATION_Update(&sys->orientation);
	POSITION_Update(&sys->position);

	//Regulation
	REGULATION_POSITION_Process();
	REGULATION_ORIENTATION_Process();
	PROPULSION_Update_Motors();
}

void process_task_scheduler(uint32_t current_time_us){
	SCHEDULER_task();
}

void process_controller_channel_update(uint32_t current_time_us){
	if(CONTROLLER_Update_Channels_Array())
		SCHEDULER_task_set_mode(TASK_CONTROLLER_CHANNEL_ANALYSIS, TASK_MODE_EVENT);
}

void process_controller_channel_analysis(uint32_t current_time_us){
	CONTROLLER_Update_Channels_Analysis();
}

void process_high_lvl(uint32_t current_time_us){
	EVENT_process(TRUE);
	FLIGHT_MODE_Main(current_time_us);
}

void process_telemetry(uint32_t current_time_us){
	TELEMETRY_Process(current_time_us);
}

void process_logger(uint32_t current_time_us){
	DATA_LOGGER_Main();
}

void process_self_test(uint32_t current_time_us){
	sys->soft.cpu_load = SCHEDULER_get_cpu_load();
}

void process_baro_update(uint32_t current_time_us){
	SCHEDULER_reschedule_task(TASK_BARO_UPDATE, BARO_Main(&sys->sensors.baro));
}

void process_baro_temp(uint32_t current_time_us){
	BARO_compute_temp(&sys->sensors.baro);
}

void process_baro_pressure(uint32_t current_time_us){
	BARO_compute_pressure(&sys->sensors.baro);
	EVENT_Set_flag(FLAG_BARO_PRESSURE_RDY);
}

void process_baro_altitude(uint32_t current_time_us){
	BARO_compute_altitude(&sys->sensors.baro);
	EVENT_Set_flag(FLAG_BARO_ALTITUDE_RDY);
}

#define TASKS_START_TIME_US 2000000
#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param, mode_param) { 	\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.process = task_function_param,						\
	.desired_period_us = desired_period_us_param,		\
	.mode = mode_param,									\
	.last_execution_us = TASKS_START_TIME_US,			\
	.desired_next_start_us = TASKS_START_TIME_US + desired_period_us_param	\
}

#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)

task_t tasks [TASK_COUNT] ={
	[TASK_EVENT_CHECK] = 					DEFINE_TASK(TASK_EVENT_CHECK, 					PRIORITY_EVENT, 		process_event_main, 		PERIOD_US_FROM_HERTZ(1), 					TASK_MODE_ALWAYS),
	[TASK_SCHEDULER] = 						DEFINE_TASK(TASK_SCHEDULER, 					PRIORITY_SCHEDULER, 	process_task_scheduler, 	PERIOD_US_FROM_HERTZ(1), 					TASK_MODE_ALWAYS),

	[TASK_PRINTF] = 						DEFINE_TASK(TASK_PRINTF, 						PRIORITY_LOW, 			process_print_f, 			PERIOD_US_FROM_HERTZ(1), 					TASK_MODE_TIME),
	[TASK_LED] = 							DEFINE_TASK(TASK_LED, 							PRIORITY_MEDIUM,	 	process_led, 				PERIOD_US_FROM_HERTZ(200), 					TASK_MODE_TIME),

	[TASK_GYRO_UPDATE] = 					DEFINE_TASK(TASK_GYRO_UPDATE, 					PRIORITY_REAL_TIME,	 	process_gyro_update, 		PERIOD_US_FROM_HERTZ(GYRO_FREQUENCY), 		TASK_MODE_TIME),
	[TASK_GYRO_FILTER] = 					DEFINE_TASK(TASK_GYRO_FILTER, 					PRIORITY_REAL_TIME,	 	process_gyro_filter, 		PERIOD_US_FROM_HERTZ(1), 					TASK_MODE_WAIT),

	[TASK_ACC_UPDATE] = 					DEFINE_TASK(TASK_ACC_UPDATE, 					PRIORITY_LOW,	 		process_acc_update, 		PERIOD_US_FROM_HERTZ(500), 					TASK_MODE_TIME),
	[TASK_ACC_FILTER] = 					DEFINE_TASK(TASK_ACC_FILTER, 					PRIORITY_REAL_TIME,	 	process_acc_filter, 		PERIOD_US_FROM_HERTZ(1), 					TASK_MODE_WAIT),
	[TASK_HIGH_LVL] = 						DEFINE_TASK(TASK_HIGH_LVL, 						PRIORITY_HIGH,	 		process_high_lvl, 			PERIOD_US_FROM_HERTZ(500), 					TASK_MODE_TIME),
	[TASK_TELEMETRIE] = 					DEFINE_TASK(TASK_TELEMETRIE, 					PRIORITY_MEDIUM,	 	process_telemetry, 			PERIOD_US_FROM_HERTZ(1000), 				TASK_MODE_TIME),
	[TASK_LOGGER] = 						DEFINE_TASK(TASK_LOGGER, 						PRIORITY_MEDIUM,	 	process_logger, 			PERIOD_US_FROM_HERTZ(5), 					TASK_MODE_TIME),

	[TASK_ORIENTATION_UPDATE] = 			DEFINE_TASK(TASK_ORIENTATION_UPDATE, 			PRIORITY_REAL_TIME,		process_orientation_update, 			PERIOD_US_FROM_HERTZ(1), 		TASK_MODE_WAIT),
	[TASK_CONTROLLER_CHANNEL_UPDATE] = 		DEFINE_TASK(TASK_CONTROLLER_CHANNEL_UPDATE, 	PRIORITY_HIGH,			process_controller_channel_update, 		PERIOD_US_FROM_HERTZ(1), 		TASK_MODE_WAIT),
	[TASK_CONTROLLER_CHANNEL_ANALYSIS] = 	DEFINE_TASK(TASK_CONTROLLER_CHANNEL_ANALYSIS, 	PRIORITY_HIGH,			process_controller_channel_analysis, 	PERIOD_US_FROM_HERTZ(1), 		TASK_MODE_WAIT),
	[TASK_SELF_TEST] = 						DEFINE_TASK(TASK_SELF_TEST, 					PRIORITY_MEDIUM,		process_self_test, 						PERIOD_US_FROM_HERTZ(50), 		TASK_MODE_TIME),

	[TASK_BARO_UPDATE] = 					DEFINE_TASK(TASK_BARO_UPDATE, 					PRIORITY_MEDIUM,		process_baro_update, 						PERIOD_US_FROM_HERTZ(100), 	TASK_MODE_TIME),
	[TASK_BARO_TEMP] = 						DEFINE_TASK(TASK_BARO_TEMP, 					PRIORITY_MEDIUM,		process_baro_temp, 							PERIOD_US_FROM_HERTZ(1), 	TASK_MODE_WAIT),
	[TASK_BARO_PRESSURE] = 					DEFINE_TASK(TASK_BARO_PRESSURE, 				PRIORITY_MEDIUM,		process_baro_pressure, 						PERIOD_US_FROM_HERTZ(1), 	TASK_MODE_WAIT),
	[TASK_BARO_ALTITUDE] = 					DEFINE_TASK(TASK_BARO_ALTITUDE, 				PRIORITY_MEDIUM,		process_baro_altitude, 						PERIOD_US_FROM_HERTZ(1), 	TASK_MODE_WAIT),



};


task_t * TASK_get_task(task_ids_t id){
	return &tasks[id];
}


