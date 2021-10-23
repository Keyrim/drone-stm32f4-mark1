/*
 * Flight_mode.h
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */

#ifndef FLIGHT_MODE_FLIGHT_MODE_H_
#define FLIGHT_MODE_FLIGHT_MODE_H_

#include "../OS/system_d.h"


typedef enum flight_modes_names_e{
	FLIGHT_MODE_ON_THE_GROUND = 0x00,
	FLIGHT_MODE_MANUAL_ACCRO,
	FLIGHT_MODE_MANUAL_ANGLE,
	FLIGHT_MODE_MANUAL_ALTI_AUTO,
	FLIGHT_MODE_ARMING,
	FLIGHT_MODE_GYRO_ACC_CALIBRATION,
	FLIGHT_MODE_PID_SETTINGS,
	FLIGHT_MODE_COUNT
}flight_modes_names_e;

typedef struct flight_mode_t{
	flight_modes_names_e name;
	void (*function_entrance)(system_t * sys);
	void (*function_main)(system_t * sys, uint32_t);
	void (*function_leave)(system_t * sys);
}flight_mode_t;


//	-------------------------	Prototypes	-------------------------
void FLIGHT_MODE_Init(system_t * sys_);
void FLIGHT_MODE_Set_Flight_Mode(flight_modes_names_e new);
void FLIGHT_MODE_Main(uint32_t time_us);

//	-------------------------	Prototypes	-------------------------
void on_the_ground_entrance(system_t * sys);
void on_the_ground_main(system_t * sys, uint32_t time_us);

void manual_accro_entrance(system_t * sys);
void manual_accro_main(system_t * sys, uint32_t time_us);

void manual_angle_entrance(system_t * sys);
void manual_angle_main(system_t * sys, uint32_t time_us);

void manual_alti_auto_entrance(system_t * sys);
void manual_alti_auto_main(system_t * sys, uint32_t time_us);

void arming_entrance(system_t * sys);
void arming_main(system_t * sys, uint32_t time_us);

void gyro_acc_calibration_entrance(system_t * sys);
void gyro_acc_calibration_main(system_t * sys, uint32_t time_us);

void pid_settings_entrance(system_t * sys);
void pid_settings_main(system_t * sys, uint32_t time_us);
#endif /* FLIGHT_MODE_FLIGHT_MODE_H_ */
