/*
 * mask_def.c
 *
 *  Created on: 17 sept. 2020
 *      Author: Theo
 */

#include "mask_def.h"



//Prototypes
// ----------------	GYRO ------------------------
static void mask_def_gyro_data_ready(Event_t * event);
static void mask_def_gyro_init_ok(Event_t * event);

// ----------------	ACC ------------------------
static void mask_def_acc_data_ready(Event_t * event);
static void mask_def_acc_init_ok(Event_t * event);

// ----------------	Orientation ------------------------
static void mask_def_orientation_update(Event_t * event);

// ----------------	Radio ------------------------
static void mask_def_ibus_data_rdy(Event_t * event);

// ----------------	Baro ------------------------
static void mask_def_baro_data_rdy(Event_t * event);

// ----------------	Flight Modes ------------------------
static void mask_def_on_the_ground(Event_t * event);
static void mask_def_manual_accro(Event_t * event);
static void mask_def_arming(Event_t * event);
static void mask_def_gyro_acc_calibration(Event_t * event);
static void mask_def_manual_angle(Event_t * event);
static void mask_def_manual_angle_alti_auto(Event_t * event);



void mask_def_events_init(Event_t * event){
	//Gyro mask def
	mask_def_gyro_init_ok(&event[EVENT_GYRO_INIT_OK]);
	mask_def_gyro_data_ready(&event[EVENT_GYRO_DATA_READY]);
	//Acc mask def
	mask_def_acc_init_ok(&event[EVENT_ACC_INIT_OK]);
	mask_def_acc_data_ready(&event[EVENT_ACC_DATA_READY]);
	//Ibus
	mask_def_ibus_data_rdy(&event[EVENT_IBUS_DATA_RDY]);
	//Orientation
	mask_def_baro_data_rdy(&event[EVENT_BARO_DATA_RDY]);
	//Baro
	mask_def_orientation_update(&event[EVENT_ORIENTATION_UPDATE]);
	//Flight_modes
	mask_def_on_the_ground(&event[EVENT_ON_THE_GROUND]);
	mask_def_manual_accro(&event[EVENT_MANUAL_ACCRO]);
	mask_def_arming(&event[EVENT_ARMING]);
	mask_def_gyro_acc_calibration(&event[EVENT_GYRO_ACC_CALIBRATION]);
	mask_def_manual_angle(&event[EVENT_MANUAL_ANGLE]);
	mask_def_manual_angle_alti_auto(&event[EVENT_MANUAL_ANGLE_ALTITUDE_AUTO]);

}

// ----------------	GYRO ------------------------
static void mask_def_gyro_init_ok(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_GYRO_INIT_OK], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_or[MASK_GYRO_INIT_OK], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_not[MASK_GYRO_INIT_OK], FLAG_GYRO_READING);
}

static void mask_def_gyro_data_ready(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_GYRO_DATA_READY], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_or[MASK_GYRO_DATA_READY], FLAG_GYRO_DATA_READY);
}

// ----------------	ACC ------------------------

static void mask_def_acc_init_ok(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_ACC_INIT_OK], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_or[MASK_ACC_INIT_OK], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_not[MASK_ACC_INIT_OK], FLAG_ACC_READING);
}

static void mask_def_acc_data_ready(Event_t * event){
	//Si le gyro est init et pas déjà en train de tourner
	MASK_set_flag(&event->mask_and[MASK_ACC_DATA_READY], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_or[MASK_ACC_DATA_READY], FLAG_ACC_DATA_READY);
}

// ----------------	ORIENTATION ------------------------
static void mask_def_orientation_update(Event_t * event){
	//Si les données filtrées du gyro et de l'acc on go
	MASK_set_flag(&event->mask_and[MASK_ORIENTATION_UPDATE], FLAG_ACC_OK);
	MASK_set_flag(&event->mask_and[MASK_ORIENTATION_UPDATE], FLAG_GYRO_OK);
	MASK_set_flag(&event->mask_and[MASK_ORIENTATION_UPDATE], FLAG_GYRO_FILTERED_DATA_READY);
	MASK_set_flag(&event->mask_or[MASK_ORIENTATION_UPDATE], FLAG_ACC_FILTERED_DATA_READY);
}

// ----------------	Radio ------------------------
static void mask_def_ibus_data_rdy(Event_t * event){
	//Si flag data rdy on y go
	MASK_set_flag(&event->mask_and[MASK_IBUS_DATA_RDY], FLAG_IBUS_DATA_RDY);
	MASK_set_flag(&event->mask_or[MASK_IBUS_DATA_RDY], FLAG_IBUS_DATA_RDY);
}

// ----------------	BAaro ------------------------
static void mask_def_baro_data_rdy(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_BARO_DATA_READY_TEMP_RAW], FLAG_BARO_OK);
	MASK_set_flag(&event->mask_or[MASK_BARO_DATA_READY_TEMP_RAW], FLAG_BARO_TEMP_RAW_RDY);

	MASK_set_flag(&event->mask_and[MASK_BARO_DATA_READY_PRESSURE_RAW], FLAG_BARO_OK);
	MASK_set_flag(&event->mask_or[MASK_BARO_DATA_READY_PRESSURE_RAW], FLAG_BARO_PRESSURE_RAW_RDY);

	MASK_set_flag(&event->mask_and[MASK_BARO_DATA_READY_PRESSURE], FLAG_BARO_OK);
	MASK_set_flag(&event->mask_or[MASK_BARO_DATA_READY_PRESSURE], FLAG_BARO_PRESSURE_RDY);
}

// ----------------	Flight Modes ------------------------
static void mask_def_on_the_ground(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_ON_THE_GROUND_FLYING], FLAG_FLYING);
	MASK_set_flag(&event->mask_or[MASK_ON_THE_GROUND_FLYING], FLAG_CHAN_5_POS_1);

	MASK_set_flag(&event->mask_and[MASK_ON_THE_GROUND_ARMING], FLAG_ARMING);
	MASK_set_flag(&event->mask_or[MASK_ON_THE_GROUND_ARMING], FLAG_CHAN_5_POS_1);

	MASK_set_flag(&event->mask_and[MASK_ON_THE_GROUND_CALIBRATION], FLAG_GYRO_CALI_IN_PROGRESS);
	MASK_set_flag(&event->mask_or[MASK_ON_THE_GROUND_CALIBRATION], FLAG_GYRO_CALI_DONE);


}

static void mask_def_manual_accro(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_MANUAL_ACCRO_ON_THE_GROUND], FLAG_CHAN_6_POS_1);
	MASK_set_flag(&event->mask_or[MASK_MANUAL_ACCRO_ON_THE_GROUND], FLAG_ARMED);
}

static void mask_def_manual_angle(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_MANUAL_ANGLE_ON_THE_GROUND], FLAG_CHAN_6_POS_2);
	MASK_set_flag(&event->mask_or[MASK_MANUAL_ANGLE_ON_THE_GROUND], FLAG_ARMED);
}

static void mask_def_manual_angle_alti_auto(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_MANUAL_ANGLE_ALTI_AUTO], FLAG_CHAN_6_POS_3);
	MASK_set_flag(&event->mask_or[MASK_MANUAL_ANGLE_ALTI_AUTO], FLAG_ARMED);
}

static void mask_def_arming(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_ARMING_ON_THE_GROUND], FLAG_GYRO_OK);

	MASK_set_flag(&event->mask_or[MASK_ARMING_ON_THE_GROUND], FLAG_CHAN_5_POS_3);

	MASK_set_flag(&event->mask_not[MASK_ARMING_ON_THE_GROUND], FLAG_FLYING);
	MASK_set_flag(&event->mask_not[MASK_ARMING_ON_THE_GROUND], FLAG_BUSY);


}

static void mask_def_gyro_acc_calibration(Event_t * event){
	MASK_set_flag(&event->mask_and[MASK_GYRO_ACC_CALIBRATION], FLAG_GYRO_OK);

	MASK_set_flag(&event->mask_or[MASK_GYRO_ACC_CALIBRATION], FLAG_CHAN_9_PUSH);

	MASK_set_flag(&event->mask_not[MASK_GYRO_ACC_CALIBRATION], FLAG_BUSY);
	MASK_set_flag(&event->mask_not[MASK_GYRO_ACC_CALIBRATION], FLAG_FLYING);
}



