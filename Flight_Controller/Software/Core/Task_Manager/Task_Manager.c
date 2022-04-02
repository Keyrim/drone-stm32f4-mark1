/*
 * Task_Manager.c
 *
 *  Created on: Nov 24, 2021
 *      Author: Théo Magne
 */


#include "macro_types.h"
#include "../Inc/tim.h"
#include "Task_Manager.h"

#define MAX_NUMBER_OF_TASK	15

/*
 * @brief Process structure definition
 */
typedef struct
{
	void (*call)(void);
	float duration;
	uint32_t duration_max;
	uint32_t duration_min;
}process_t;

/*
 * @brief Task Structure definition
 */
typedef struct
{
	char * name;
	uint8_t name_len;
	process_t init;
	process_t main;
	process_t it;
	process_t gyro_data_ready;
}task_t;
/*
 * @brief task manager structure definition
 */
typedef struct
{
	float duration_us_init;
	float duration_us_main;
	float duration_us_it;
	float duration_us_gyro;
	uint32_t time_ms;
	uint32_t time_us;
	TIM_HandleTypeDef * htim;
}task_manager_t;

static void PROCESS_Call(process_t * process);

static task_manager_t task_manager =
{
	.htim = &htim2
};

static task_t task[MAX_NUMBER_OF_TASK] = { 0 };
static bool_e is_initialized = FALSE;
static uint8_t task_count = 0;

void TASK_MANAGER_Init(void)
{
	HAL_TIM_Base_Start_IT(task_manager.htim);
	uint32_t tmp = 0;
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].init.call)
		{
			PROCESS_Call(&task[t].init);
			tmp += task[t].init.duration;
		}
	}
	is_initialized = TRUE;
	task_manager.duration_us_init =  tmp;
}


void TASK_MANAGER_Main(void)
{
	uint32_t tmp = 0;
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].main.call)
		{
			PROCESS_Call(&task[t].main);
			tmp += task[t].main.duration;
		}
	}
	task_manager.duration_us_main = tmp;
}

void TASK_MANAGER_It_ms(void)
{
	if(!is_initialized)
	{
		return;
	}
	uint32_t tmp = 0;
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].it.call)
		{
			PROCESS_Call(&task[t].it);
			tmp += task[t].it.duration;
		}
	}
	task_manager.duration_us_it = tmp;
}

void TASK_MANAGER_Gyro_Data_Ready(void)
{
	uint32_t tmp = 0;
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].gyro_data_ready.call)
		{
			PROCESS_Call(&task[t].gyro_data_ready);
			tmp += task[t].gyro_data_ready.duration;
		}
	}
	task_manager.duration_us_gyro = tmp;
}

uint32_t TASK_MANAGER_Get_Time_Us(void)
{
	uint32_t time;
	__disable_irq();
	time = 1000 * task_manager.time_ms + task_manager.htim->Instance->CNT;
	__enable_irq();
	return time;
}

void TASK_MANAGER_Tick_Timer(void)
{
	task_manager.time_ms ++;
}

void TASK_MANAGER_Add_Task(char * name, void(*init)(void), void(*main)(void), void(*it)(void), void(*gyro)(void))
{
	if(task_count == MAX_NUMBER_OF_TASK)
	{
		return;
	}
	task[task_count].name = name;
	task[task_count].name_len = sizeof(name) - 1;
	task[task_count].init.call = init;
	task[task_count].main.call = main;
	task[task_count].it.call = it;
	task[task_count++].gyro_data_ready.call = gyro;
}

static void PROCESS_Call(process_t * process)
{
	uint32_t time = TASK_MANAGER_Get_Time_Us();
	process->call();
	time = TASK_MANAGER_Get_Time_Us() - time;
	process->duration_min = MIN(time, process->duration_min);
	process->duration_max = MAX(time, process->duration_max);
	process->duration = process->duration * 0.998f + time * 0.002;
}


