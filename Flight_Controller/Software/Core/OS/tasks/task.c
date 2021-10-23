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
}

/* Task function prototypes */
static void process_scheduler(uint32_t current_time_us);
static void process_events(uint32_t current_time_us);

task_t tasks [task_ids_eCOUNT] =
{
		[task_ids_eSCHEDULER] = DEFINE_TASK(task_ids_eSCHEDULER,
										task_priority_eSCHEDULER,
										process_scheduler,
										PERIOD_US_FROM_HERTZ(1),
										task_mode_eALWAYS
										),
		[task_ids_eEVENTS] = DEFINE_TASK(task_ids_eEVENTS,
										task_priority_eEVENT,
										process_events,
										PERIOD_US_FROM_HERTZ(1),
										task_mode_eALWAYS
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

/* Public functions */
/*
 * @brief
 */
task_t * TASK_get_task(task_ids_e id)
{
	return &tasks[id];
}


