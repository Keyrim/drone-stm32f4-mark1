/*
 * Task_Manager.h
 *
 *  Created on: Nov 24, 2021
 *      Author: Théo Magne
 */

#ifndef TASK_MANAGER_H_
#define TASK_MANAGER_H_


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

void TASK_MANAGER_Init(void);
void TASK_MANAGER_Main(void);
void TASK_MANAGER_It_ms(void);
void TASK_MANAGER_Gyro_Data_Ready(void);
void TASK_MANAGER_Add_Task(char * name, void(*init)(void), void(*main)(void), void(*it)(void), void(*gyro)(void));

#endif /* TASK_MANAGER_H_ */
