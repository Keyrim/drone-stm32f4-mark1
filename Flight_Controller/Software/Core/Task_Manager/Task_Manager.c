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
	uint32_t duration_tick;
	float duration;
	float duration_raw;
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
	TIM_HandleTypeDef * htim_main;
	float tick_main_to_us;
	TIM_HandleTypeDef * htim_it;
	float tick_it_to_us;
}task_manager_t;

static float PROCESS_Call(process_t * process, TIM_HandleTypeDef * htim);

static task_manager_t task_manager =
{
	.htim_main = &htim2,
	.htim_main = &htim5,

};

static task_t task[MAX_NUMBER_OF_TASK] = { 0 };
static bool_e is_initialized = FALSE;
static uint8_t task_count = 0;

void TASK_MANAGER_Init(void)
{

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
	uint32_t previous_tick = 0;
	uint32_t tick;
	HAL_TIM_Base_Start(task_manager.htim_main);
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].main.call)
		{
			previous_tick = PROCESS_Call(&task[t].main);
			tick = task_manager.htim_main->Instance->CNT;
			task[t].main.duration_tick = tick - previous_tick;
			task[t].main.duration_raw = task[t].main.duration_tick * task_manager.tick_main_to_us;
			task[t].main.duration = task[t].main.duration * 0.998f + task[t].main.duration_raw * 0.002f;
			previous_tick = tick;
		}
	}
	HAL_TIM_Base_Stop(task_manager.htim_main);
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

static float PROCESS_Call(process_t * process, TIM_HandleTypeDef * htim)
{
	tick = htim->Instance->CNT;
	process->call();
	process->duration_tick = htim->Instance->CNT - tick;
	process->duration_raw = process->duration_tick * task_manager.tick_main_to_us;
	process->duration = process->duration * 0.998f + process->duration_raw * 0.002f;
	return process->duration_raw;
}


