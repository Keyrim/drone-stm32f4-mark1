/*
 * scheduler.c
 *
 *  Created on: Oct 23, 2021
 *      Author: Theo
 */
#include "scheduler.h"
#include "../time.h"

/* Privates variables */
static task_t * task_queu[task_ids_eCOUNT];
static uint32_t task_queu_size = 0 ;
static uint32_t task_queu_position = 0 ;
static task_t * task_to_change[task_ids_eCOUNT] ;
static task_mode_e change_mode[task_ids_eCOUNT] ;
static uint8_t task_to_moove_counter;
static task_t * task_scheduler;
static task_t * task_event;

/* ----------------------- Private functions prototypes -------------------- */
static task_t * get_first_task(void);
static task_t * get_next_task(void);
static uint32_t task_process(task_t * task, uint32_t current_time_us);
static bool_e queu_contains(task_t * task);
static bool_e queu_add(task_t * task);
static bool_e queu_remove(task_t * task);




/* ----------------------- Public functions -------------------- */

/*
 * @brief Must be called after TASK_init function
 * 	      Store the pointer to the scheduler and event task to buy time
 */
void SCHEDULER_init(void)
{
	task_scheduler = TASK_get_task(task_ids_eSCHEDULER);
	task_event = TASK_get_task(task_ids_eEVENTS);
}

/*
 * @brief It calls tasks one after another according to their priorities and current modes
 */
void SCHEDULER_run(void)
{
	uint32_t current_time_us = TIME_us();
	task_t * task = get_first_task();
	bool_e task_executed = FALSE ;
	while(task_queu_position < task_queu_size && task != NULL)
	{

		switch(task->mode)
		{
			case task_mode_eALWAYS:
				current_time_us = task_process(task, current_time_us);
				task_executed = TRUE ;
				break;
			case task_mode_eTIME :
				if(current_time_us >= task->desired_next_start_us){
					current_time_us = task_process(task, current_time_us);
					task->desired_next_start_us = task->desired_period_us - (int32_t)task->lag_average + current_time_us ;
					task_executed = TRUE ;
				}
				break;
			case task_mode_eEVENT :
				current_time_us = task_process(task, current_time_us);
				task_executed = TRUE ;
				task->mode = task_mode_eWAIT ;
				break;
			case task_mode_eTIMMER :
			case task_mode_eWAIT:
			default:
				break;
		}
		/* Stop the while loop when once a task gets executed */
		if(task_executed && (task != task_scheduler) && (task != task_event))
		{
			break;
		}
		task_executed = FALSE ;
		task = get_next_task();
	}
}

/*
 * @brief Check for some tasks that requires to change of mode
 */
void SCHEDULER_task(void)
{
	for(uint8_t t = 0; t < task_to_moove_counter; t++)
	{
		task_to_change[t]->mode = change_mode[t] ;
	}
	task_to_moove_counter = 0 ;
}

/*
 * @brief Compute and return the current cpu load
 * @return the current cpu load in %
 */

float SCHEDULER_get_cpu_load(void)
{
	float load_pourcentage = 0 ;
	for(uint32_t t = 2; t < task_queu_size; t++)
		if(task_queu[t]->real_period_us)
			load_pourcentage += task_queu[t]->duration_us * 51200 / task_queu[t]->real_period_us ;

	return load_pourcentage / 512.0f ;
}

/*
 * @brief Add or remove a task to / from the queue
 * @param id The id of the task
 * @param enable Enable (Add) or disable (remove) a task
 */
void SCHEDULER_enable_task(task_ids_e id, bool_e enable)
{
	if(enable)
	{
		queu_add(TASK_get_task(id));
	}
	else
	{
		queu_remove(TASK_get_task(id));
	}
}

/*
 * @brief Change the execution mode of a task
 * @param id Id of the task
 * @param mode The mode we want the task to be in
 */
void SCHEDULER_task_set_mode(task_ids_e id, task_mode_e mode)
{
	__disable_irq();
	task_to_change[task_to_moove_counter] = TASK_get_task(id);
	change_mode[task_to_moove_counter++] = mode ;
	__enable_irq();
}

/*
 * @brief Change the execution period of a task
 * @param id The id of the task
 * @param new_period_us The new desired period
 */
void SCHEDULER_reschedule_task(task_ids_e id, uint32_t new_period_us)
{
	task_t * task = TASK_get_task(id);
	task->desired_period_us = new_period_us ;
}

/* ----------------------- Private functions -------------------- */

/*
 * @brief get the first task of the queue
 */
static task_t * get_first_task(void)
{
	task_queu_position = 0 ;
	return task_queu[0] ;
}

/*
 * @brief get the next task in the queue
 * @return the next task
 */
static task_t * get_next_task(void)
{
	return task_queu[++task_queu_position];
}

static uint32_t task_process(task_t * task, uint32_t current_time_us){

	/* If the task has no process associated, we just stop here before making a mistake .. */
	if(task->process == NULL)
	{
		return current_time_us ;
	}

	/* Compute the real period using the moving average */
	task->real_period_us_average_sum -= task->real_period_us_average_array[task->average_index];
	task->real_period_us_average_array[task->average_index] = current_time_us - task->last_execution_us ;
	task->real_period_us_average_sum += task->real_period_us_average_array[task->average_index] ;
	task->real_period_us = task->real_period_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	/* Lag compensation */
	if(task->mode == task_mode_eTIME)
	{
		if(task->real_period_us_average_array[task->average_index] > task->desired_period_us)
		{
			task->lag_average += (float)(task->real_period_us_average_array[task->average_index] - task->desired_period_us) * 0.5f ;
		}

		else
		{
			task->lag_average -= (float)(task->desired_period_us - task->real_period_us_average_array[task->average_index]) * 0.5f ;
		}
	}
	task->last_execution_us = current_time_us ;
	task->process(current_time_us);
	current_time_us = TIME_us();
	/* Compute the "burst time" with the moving average */
	task->duration_us_average_sum -= task->duration_us_average_array[task->average_index];
	task->duration_us_average_array[task->average_index] = current_time_us - task->last_execution_us ;
	task->duration_us_worst = MAX(task->duration_us_average_array[task->average_index] , task->duration_us_worst);
	task->duration_us_average_sum += task->duration_us_average_array[task->average_index];
	task->duration_us = task->duration_us_average_sum / TASK_STAT_AVERAGE_OVER ;

	if(task->duration_us_average_array[task->average_index] > 1000)
		task->duration_us_average_array[task->average_index] ++ ;

	if(++task->average_index == TASK_STAT_AVERAGE_OVER)
		task->average_index = 0 ;
	return current_time_us ;
}


/*
 * @brief Check if the given task is currently in the queue
 * @param task pointer to a task
 * @return true when it does contains the task, false otherwise
 */
static bool_e queu_contains(task_t * task)
{
	if(task == NULL)
	{
		return FALSE ;
	}
	for(uint32_t t = 0; t < task_queu_size; t++)
	{
		if(task_queu[t] == task)
			{
				return TRUE ;
			}
	}

	return FALSE ;
}

/*
 * @brief Add a task to the queue
 * @param task pointer to the task to add
 * @return true when added, false otherwise
 */
static bool_e queu_add(task_t * task)
{

	if(queu_contains(task) || task_queu_size >= task_ids_eCOUNT || task == NULL)
	{
		return FALSE ;
	}
	uint32_t t = 0 ;
	while(t < task_ids_eCOUNT)
	{
		if(task_queu[t] == NULL)
		{
			task_queu[t] = task ;
			task_queu_size ++ ;
			return TRUE;
		}
		else if(task->static_priority > task_queu[t]->static_priority){
			memmove(&task_queu[t+1], &task_queu[t], sizeof(task) * (task_queu_size - t));
			task_queu[t] = task ;
			task_queu_size ++ ;
			return TRUE ;
		}
		t++;
	}
	return FALSE ;
}
/*
 * @brief remove a task from the queue
 * @param task pointer to the task to remove
 * @return true if the task was removed, false otherwise
 */
static bool_e queu_remove(task_t * task)
{
	if(!queu_contains(task) || task == NULL)
	{
		return FALSE ;
	}
	for(uint32_t t = 0; t < task_queu_size; t ++)
	{
		if(task == task_queu[t])
		{
			memmove(&task_queu[t], &task_queu[t+1], sizeof(task) * (task_queu_size - t));
			task_queu_size -- ;
			return TRUE ;
		}
	}
	return FALSE ;
}
