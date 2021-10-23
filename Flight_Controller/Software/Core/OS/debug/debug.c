/*
 * debug.c
 *
 *  Created on: Nov 21, 2020
 *      Author: Théo
 */



#include "debug.h"
#include "retarget.h"

extern UART_HandleTypeDef huart6 ;

void DEBUG_init(void){
	RetargetInit(&huart6);
	printf("Debuger is init\n");
}

