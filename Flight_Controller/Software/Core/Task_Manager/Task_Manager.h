/*
 * Task_Manager.h
 *
 *  Created on: Nov 24, 2021
 *      Author: Théo Magne
 */

#ifndef TASK_MANAGER_H_
#define TASK_MANAGER_H_


void TASK_MANAGER_Init(void);
void TASK_MANAGER_Main(void);
void TASK_MANAGER_It_ms(void);
void TASK_MANAGER_Gyro_Data_Ready(void);
void TASK_MANAGER_Add_Task(char * name, void(*init)(void), void(*main)(void), void(*it)(void), void(*gyro)(void));

#endif /* TASK_MANAGER_H_ */
