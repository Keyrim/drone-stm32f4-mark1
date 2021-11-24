/*
 * task.c
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */

#include "task.h"
#include "../scheduler/scheduler.h"
#include "../events/events.h"
#include "../OS/time.h"
#include "../../config.h"
#include "../../Peripherals/Uart.h"
#include "../../Data_Logger/Data_logger.h"
#include "../../Sensors/Mpu.h"
#include "../../Complementary_Filter/Complementary_Filter.h"
#include "../../Motors/Motors.h"

#define TASKS_START_TIME_US 2000000
#define PERIOD_US_FROM_HERTZ(hertz_param) (1000000 / hertz_param)
#define DEFINE_TASK(id_param, priority_param,  task_function_param, desired_period_us_param, mode_param) \
{ 														\
	.id = id_param,										\
	.static_priority = priority_param,					\
	.process = task_function_param,						\
	.desired_period_us = desired_period_us_param,		\
	.mode = mode_param,									\
	.last_execution_us = TASKS_START_TIME_US,			\
	.desired_next_start_us = TASKS_START_TIME_US + desired_period_us_param	\
}

/* The main system structure used by most of the tasks */
static system_t * mark1;

/*
 * @brief Initialization of every task
 * @param mark1_ pointer to a system_t structure
 */
void TASK_Init(system_t * mark1_)
{
	mark1 = mark1_;
	SCHEDULER_enable_task(task_ids_eEVENTS, TRUE);
	SCHEDULER_enable_task(task_ids_eSCHEDULER, TRUE);
	SCHEDULER_enable_task(task_ids_eGYRO_UPDATE, TRUE);
	SCHEDULER_enable_task(task_ids_eDATA_LOGGER, TRUE);
}

/* Task function prototypes */
static void process_scheduler(uint32_t current_time_us);
static void process_events(uint32_t current_time_us);
static void process_data_logger(uint32_t current_time_us);
static void process_gyro_update(uint32_t current_time_us);

/* Tasks definition */
task_t tasks [task_ids_eCOUNT] =
{
		[task_ids_eSCHEDULER] = 		DEFINE_TASK(task_ids_eSCHEDULER,
													task_priority_eSCHEDULER,
													process_scheduler,
													PERIOD_US_FROM_HERTZ(1),
													task_mode_eALWAYS
													),
		[task_ids_eEVENTS] =	 		DEFINE_TASK(task_ids_eEVENTS,
													task_priority_eEVENT,
													process_events,
													PERIOD_US_FROM_HERTZ(1),
													task_mode_eALWAYS
													),
		[task_ids_eDATA_LOGGER] = 		DEFINE_TASK(task_ids_eDATA_LOGGER,
													task_priority_eHIGH,
													process_data_logger,
													PERIOD_US_FROM_HERTZ(1),
													task_mode_eTIME
													),
		[task_ids_eGYRO_UPDATE] = 		DEFINE_TASK(task_ids_eGYRO_UPDATE,
													task_priority_eREAL_TIME,
													process_gyro_update,
													PERIOD_US_FROM_HERTZ(100),
													task_mode_eTIME
													),


};

/* ----------- Task functions --------------- */

/*
 * @brief Call the scheduler main function
 */
static void process_scheduler(uint32_t current_time_us)
{
	SCHEDULER_task();
}

/*
 * @brief Call the scheduler main function
 */
static void process_events(uint32_t current_time_us)
{
	UNUSED(current_time_us);
	EVENT_process(FALSE);
}

/*
 * @brief Call the data logger main function
 */
static void process_data_logger(uint32_t current_time_us)
{
	UNUSED(current_time_us);
	DATA_LOGGER_Main();
}

/*
 * @brief Measure new data from the gyro
 */
static void process_gyro_update(uint32_t current_time_us)
{
	MPU_Read_All();
	COMPLEMENTARY_FILTER_Process();
}

/* ---------------- Public functions ------------------ */

/*
 * @brief Return a pointer to a task for a given task id
 * @param id Id of the task
 * @return Pointer to the task
 */
task_t * TASK_get_task(task_ids_e id)
{
	return &tasks[id];
}


