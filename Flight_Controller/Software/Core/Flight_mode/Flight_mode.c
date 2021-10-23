/*
 * Flight_mode.c
 *
 *  Created on: Dec 13, 2020
 *      Author: Théo
 */



#include "Flight_mode.h"

static system_t * sys ;
static flight_modes_names_e current_flight_mode = FLIGHT_MODE_ON_THE_GROUND;
static flight_modes_names_e next_flight_mode = FLIGHT_MODE_ON_THE_GROUND;



//	-------------------------	Define flights modes	-------------------------
#define DEF_FLIGHT_MODE(name_param, function_entrance_param, function_main_param) { 	\
	.name = name_param,																	\
	.function_main = function_main_param,												\
	.function_entrance = function_entrance_param										\
}

static flight_mode_t flights_modes[FLIGHT_MODE_COUNT] = {
		[FLIGHT_MODE_ON_THE_GROUND] = 			DEF_FLIGHT_MODE(FLIGHT_MODE_ON_THE_GROUND, 				on_the_ground_entrance, 		on_the_ground_main),
		[FLIGHT_MODE_MANUAL_ACCRO] = 			DEF_FLIGHT_MODE(FLIGHT_MODE_MANUAL_ACCRO, 				manual_accro_entrance, 			manual_accro_main),
		[FLIGHT_MODE_MANUAL_ANGLE] = 			DEF_FLIGHT_MODE(FLIGHT_MODE_MANUAL_ANGLE, 				manual_angle_entrance, 			manual_angle_main),
		[FLIGHT_MODE_MANUAL_ALTI_AUTO] = 		DEF_FLIGHT_MODE(FLIGHT_MODE_MANUAL_ALTI_AUTO, 			manual_alti_auto_entrance, 		manual_alti_auto_main),
		[FLIGHT_MODE_ARMING] = 					DEF_FLIGHT_MODE(FLIGHT_MODE_ARMING, 					arming_entrance, 				arming_main),
		[FLIGHT_MODE_GYRO_ACC_CALIBRATION] = 	DEF_FLIGHT_MODE(FLIGHT_MODE_GYRO_ACC_CALIBRATION, 		gyro_acc_calibration_entrance, 	gyro_acc_calibration_main),

};

//	-------------------------	Public functions	-------------------------
void FLIGHT_MODE_Init(system_t * sys_){
	sys = sys_;
	sys->soft.flight_mode = &current_flight_mode;
}

//Dont call it from an it plz for now ..
void FLIGHT_MODE_Set_Flight_Mode(flight_modes_names_e new){
	next_flight_mode = new ;
}

void FLIGHT_MODE_Main(uint32_t time_us){
	//Check if we have to change the current flight mode
	if(next_flight_mode != current_flight_mode){

		//Call the leave function associated to the current(previous) flight mode if it exists
		if(flights_modes[current_flight_mode].function_leave != NULL)
			flights_modes[current_flight_mode].function_leave(sys);

		//Call the entrance function associated to the next flight mode if it exists
		if(flights_modes[next_flight_mode].function_entrance != NULL)
			flights_modes[next_flight_mode].function_entrance(sys);

		//Change the current flight mode
		current_flight_mode = next_flight_mode ;
	}
	//Call the function associated to the current flight mode
	if(flights_modes[current_flight_mode].function_main != NULL)
		flights_modes[current_flight_mode].function_main(sys, time_us);

}



//	-------------------------	Functions definitions	-------------------------






















