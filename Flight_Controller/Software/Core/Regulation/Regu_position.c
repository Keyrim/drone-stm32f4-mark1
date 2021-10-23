/*
 * Regu_position.c
 *
 *  Created on: 15 déc. 2020
 *      Author: Théo
 */


#include "Regu_position.h"

static regu_position_t * regu_position ;

//	--------------------------------	Velocity pid settings	-------------------------------
float pid_velocity_z_settings[PID_NB_SETTINGS] = {40.0f, 30.0f, 0.0f, REGU_POSITION_FREQUENCY, 300};
float filter_pid_velocity_z[3] = {0.04f, 1.6f, -0.64f};

float pid_position_z_settings[PID_NB_SETTINGS] = {0.0f, 0.0f, 0.0f, REGU_POSITION_FREQUENCY, 30};
float filter_pid_position_z[3] = {0.04f, 1.6f, -0.64f};

void REGULATION_POSITION_Init(regu_position_t * regu_position_, position_t * position, int16_t * outputs){
	regu_position = regu_position_;
	regu_position->mode = REGULATION_POSITION_MODE_OFF ;
	regu_position->outputs = outputs ;
	regu_position->position = position ;

	//Init pid velocity
	PID_init(&regu_position->pid_velocity[POSITION_AXE_Z], pid_velocity_z_settings, FILTER_NO_FILTERING, filter_pid_velocity_z);

	//Init pid position
	PID_init(&regu_position->pid_position[POSITION_AXE_Z], pid_position_z_settings, FILTER_SECOND_ORDER, filter_pid_position_z);


}

void REGULATION_POSITION_Set_Regulation_Mode(regu_position_mode_e regu_mode){
	regu_position->mode = regu_mode ;
}

void REGULATION_POSITION_Process(void){
	switch(regu_position->mode){
		case REGULATION_POSITION_MODE_OFF:
			regu_position->outputs[PROP_CONSIGNE_THRUST] = 0;
			break;
		case REGULATION_POSITION_MODE_ON:
			regu_position->outputs[PROP_CONSIGNE_THRUST] = regu_position->consigne;
			break;
		case REGULATION_POSITION_MODE_STABILIZED:
			regu_position->consigne_velocity[POSITION_AXE_Z] = 0; //- PID_compute(&regu_position->pid_position[POSITION_AXE_Z], regu_position->consigne_position[POSITION_AXE_Z],  regu_position->position->position[POSITION_AXE_Z]);
			regu_position->outputs[PROP_CONSIGNE_THRUST] = regu_position->consigne - PID_compute(&regu_position->pid_velocity[POSITION_AXE_Z], regu_position->consigne_velocity[POSITION_AXE_Z] , regu_position->position->velocity[POSITION_AXE_Z]);
			break;
	}
}






























