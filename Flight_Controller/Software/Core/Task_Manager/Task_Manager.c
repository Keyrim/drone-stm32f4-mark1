/*
 * Task_Manager.c
 *
 *  Created on: Nov 24, 2021
 *      Author: Théo Magne
 */


#include "macro_types.h"
#include "Task_Manager.h"
#include "../Peripherals/Timer.h"

#define MAX_NUMBER_OF_TASK	15

/*
 * @brief Process structure definition
 */
typedef struct
{
	void (*call)(void);
	int32_t duration_tick;
	float duration;
	int32_t duration_max;
	int32_t duration_min;
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
	timer_e timer_main;
	timer_e timer_it;
	timer_e timer_gyro;
}task_manager_t;

static int32_t PROCESS_Call(process_t * process, timer_e timer_id);

static task_manager_t task_manager =
{
	.timer_main = timer_e9,
	.timer_it = timer_e10,
	.timer_gyro = timer_e11

};

static task_t task[MAX_NUMBER_OF_TASK] = { 0 };
static bool_e is_initialized = FALSE;
static uint8_t task_count = 0;

void TASK_MANAGER_Init(void)
{
	TIMER_Start(task_manager.timer_main);
	TIMER_Start(task_manager.timer_it);
	TIMER_Start(task_manager.timer_gyro);

	uint32_t tmp = 0;
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].init.call)
		{
			tmp += PROCESS_Call(&task[t].init, task_manager.timer_main);
		}
	}
	is_initialized = TRUE;
	task_manager.duration_us_init =  tmp;
}


void TASK_MANAGER_Main(void)
{

	uint32_t sum = 0;
	for(uint8_t t = 0; t < task_count; t++)
	{
		if(task[t].main.call)
		{
			sum += PROCESS_Call(&task[t].main, task_manager.timer_main);
		}
	}
	task_manager.duration_us_main =  sum;
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
			tmp += PROCESS_Call(&task[t].it, task_manager.timer_it);
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
			tmp += PROCESS_Call(&task[t].gyro_data_ready, task_manager.timer_gyro);
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


static int32_t PROCESS_Call(process_t * process, timer_e timer_id)
{
	int32_t before = (int32_t)TIMER_Get_Tick(timer_id);
	process->call();
	int32_t after = (int32_t)TIMER_Get_Tick(timer_id);
	process->duration_tick =  after - before;
	if(after < before)
	{
		process->duration_tick -= (int32_t)0xffff;
	}
	process->duration = process->duration * 0.998f + (float)process->duration_tick * 0.002f;
	return process->duration_tick;
}


