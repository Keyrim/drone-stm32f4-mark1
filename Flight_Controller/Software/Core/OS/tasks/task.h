/*
 * task.h
 *
 *  Created on:  Oct 23, 2021
 *      Author: Theo
 */

#ifndef TASKS_TASK_H_
#define TASKS_TASK_H_
#include "stdint.h"
#include "stdio.h"
#include "../system_d.h"

#define TASK_STAT_AVERAGE_OVER 	20		/* Period for the moving average */
#define TASK_DURATION_MAX		800		/* Max execution time for a task */

/*
 * @brief Task priority enumeration definition
 */
typedef enum
{
	/* Higher value means higher priority
	 * Positives values only */
	task_priority_eLOW			= 1,
	task_priority_eMEDIUM	 	= 5,
	task_priority_eHIGH			= 10,
	task_priority_eREAL_TIME	= 15,
	task_priority_eSCHEDULER	= 100,
	task_priority_eEVENT		= 101
}task_priority_e;

/*
 * @brief tasks enumeration definition
 */
typedef enum
{
	task_ids_eSCHEDULER,
	task_ids_eEVENTS,
	task_ids_eUART_TEST,
	task_ids_eDATA_LOGGER,
	task_ids_eCOUNT
}task_ids_e;

/*
 * @brief Task mode definition
 */
typedef enum
{
	task_mode_eWAIT = 0x00,	//Task is in idle mode, i.e never called, wait mode has to be 0 cuz tasks start in the wait mode by default
	task_mode_eTIME,		//Execute the task at a given frequency
	task_mode_eEVENT,		//Execute the task once and then goes to wait mode
	task_mode_eALWAYS,		//Always executed
	task_mode_eTIMMER		//Execute in a timer routine and not in the main scheduler
}task_mode_e;

typedef struct task
{
	/* Task priority */
	task_priority_e static_priority ;
	task_ids_e id ;
	task_mode_e mode ;
	/* Process */
	void (*process)(uint32_t current_time_us);
	/* Average lag */
	float lag_average ;
	/* Statistics */
	uint32_t duration_us ;
	uint32_t duration_us_worst ;
	uint32_t desired_period_us ;
	uint32_t real_period_us ;
	uint32_t last_execution_us ;
	uint32_t desired_next_start_us ;
	/* Moving average arrays */
	uint32_t real_period_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t real_period_us_average_sum ;
	uint32_t duration_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t duration_us_average_sum ;
	uint32_t average_index ;
}task_t;

void TASK_Init(system_t * mark1_);
task_t * TASK_get_task(task_ids_e id);

#endif /* TASKS_TASK_H_ */
