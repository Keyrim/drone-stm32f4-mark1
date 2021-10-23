/*
 * Regu_position.h
 *
 *  Created on: 15 déc. 2020
 *      Author: Théo
 */

#ifndef REGULATION_REGU_POSITION_H_
#define REGULATION_REGU_POSITION_H_

#include "../config.h"
#include "stm32f4xx_hal.h"
#include "Regu_orientation.h"
#include "../Propulsion/Propulsion.h"
#include "../Estimators/position.h"
#include "../../Drivers/Inc/Pid.h"


#define REGU_POSITION_FREQUENCY GYRO_FREQUENCY

typedef enum regu_position_mode_e{
	REGULATION_POSITION_MODE_OFF,
	REGULATION_POSITION_MODE_ON,
	REGULATION_POSITION_MODE_STABILIZED
}regu_position_mode_e;

typedef struct regu_position_t{

	position_t * position ;

	float consigne_position[POSITION_AXES_COUNT];
	float consigne_velocity[POSITION_AXES_COUNT];
	PID_t pid_velocity[POSITION_AXES_COUNT];
	PID_t pid_position[POSITION_AXES_COUNT];

	int16_t consigne ;
	int16_t * outputs;
	regu_position_mode_e mode;

}regu_position_t;

void REGULATION_POSITION_Init(regu_position_t * regu_position_, position_t * position, int16_t * outputs);
void REGULATION_POSITION_Set_Regulation_Mode(regu_position_mode_e regu_mode);
void REGULATION_POSITION_Process(void);

#endif /* REGULATION_REGU_POSITION_H_ */
