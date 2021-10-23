/*
 * task.h
 *
 *  Created on: 13 août 2020
 *      Author: Théo
 */

#ifndef TASKS_TASK_H_
#define TASKS_TASK_H_
#include "stdint.h"
#include "stdio.h"
#include "../system_d.h"

#define TASK_STAT_AVERAGE_OVER 20		//Moyenne glissante sur X valeurs
#define TASK_DURATION_MAX	800	//On considère  qu une tache ne peut durer plus de 800µS sinon il y a un GROS problème


typedef enum task_priority{
	//Priorité toujours positives
	PRIORITY_LOW		= 1 ,
	PRIORITY_MEDIUM	 	= 5 ,
	PRIORITY_HIGH		= 10,
	PRIORITY_REAL_TIME	= 15,
	PRIORITY_SCHEDULER	= 100,
	PRIORITY_EVENT	= 101
}task_priority_t;

typedef enum task_states{
	INIT,				//	Tâche
	READY				// 	Tâches placées dans la file d attente pour être trait�
}task_states_t;

typedef enum task_ids {
	TASK_SCHEDULER,
	TASK_EVENT_CHECK,
	TASK_PRINTF,
	TASK_LED,
	TASK_GYRO_UPDATE,
	TASK_GYRO_FILTER,
	TASK_ACC_UPDATE,
	TASK_ACC_FILTER,
	TASK_ORIENTATION_UPDATE,
	TASK_CONTROLLER_CHANNEL_UPDATE,
	TASK_CONTROLLER_CHANNEL_ANALYSIS,
	TASK_HIGH_LVL,
	TASK_TELEMETRIE,
	TASK_LOGGER,
	TASK_SELF_TEST,
	TASK_BARO_UPDATE,
	TASK_BARO_TEMP,
	TASK_BARO_PRESSURE,
	TASK_BARO_ALTITUDE,
	TASK_COUNT
}task_ids_t;

typedef enum task_mode_e{
	TASK_MODE_WAIT = 0x00,	//Task is in iddle mode, i.e never called, wait mode has to be 0 cuz tasks start in the wait mode by default
	TASK_MODE_TIME,			//Execute the task at a given frequency
	TASK_MODE_EVENT,		//Execute the task once and then goes to wait mode
	TASK_MODE_ALWAYS,		//Always executed
	TASK_MODE_TIMMER		//Execute in a timmer routine and not in the main scheduler
}task_mode_e;


typedef enum{
	PROCESS_IT = 0x00,
	PROCESS_MAIN
}process_type_e;

typedef struct task{
	//Propriétées de la tâche
	task_priority_t static_priority ;
	task_ids_t id ;

	//Process
	void (*process)(uint32_t current_time_us);

	//Execution mode => defines what "starts" a task
	task_mode_e mode ;

	float lag_average ;

	//stat
	uint32_t duration_us ;
	uint32_t duration_us_worst ;
	uint32_t desired_period_us ;
	uint32_t real_period_us ;
	uint32_t last_execution_us ;
	uint32_t desired_next_start_us ;

	//Tableau pour avoir les stats en moyenne glissante
	uint32_t real_period_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t real_period_us_average_sum ;
	uint32_t duration_us_average_array [TASK_STAT_AVERAGE_OVER];
	uint32_t duration_us_average_sum ;
	uint32_t average_index ;
}task_t;

void tasks_init(system_t * sys_);
task_t * TASK_get_task(task_ids_t id);

#endif /* TASKS_TASK_H_ */
