/*
 * system_d.h
 *
 *  Created on: Nov 21, 2020
 *      Author: Théo
 */

#ifndef OS_SYSTEM_D_H_
#define OS_SYSTEM_D_H_

#include "macro_types.h"
#include "stm32f4xx_hal.h"
#include "../Sensors/gyro.h"
#include "../Sensors/acc.h"
#include "../Sensors/ms5611.h"
#include "../Sensors/baro.h"
#include "../sensors/batterie.h"
#include "../Estimators/orientation.h"
#include "../Estimators/position.h"
#include "../Radio/controller.h"
#include "../Propulsion/Propulsion.h"
#include "../Regulation/Regu_orientation.h"
#include "../Regulation/Regu_position.h"
#include "../Telemetry/Telemetry.h"
#include "../Ihm/Ihm.h"
#include "../../Drivers/Inc/Ibus.h"
#include "../../Drivers/Inc/Motors.h"
#include "../config.h"

typedef struct sensors_t{
	//Sensors
	gyro_t gyro ;
	acc_t acc ;
	batterie_t batterie ;
	baro_t baro;


	//Sensors list
	mpu_t mpu ;
	ms5611_t ms5611 ;
}sensors_t;

typedef struct regulation_t{
	//Orientation
	regu_orientation_t orientation ;
	//Position
	regu_position_t position;
}regulation_t;



typedef struct radio_t{
	ibus_t ibus;
	controller_t controller ;
	telemetry_t telemetry ;
}radio_t;

typedef struct soft_t{
	uint8_t * flight_mode ;
	float cpu_load ;
}soft_t;



typedef struct{
	Ihm_t ihm ;
	sensors_t sensors ;
	orientation_t orientation ;
	position_t position ;
	radio_t	radio;
	propulsion_t propulsion ;
	regulation_t regulation ;
	soft_t soft;
}system_t;


#endif /* OS_SYSTEM_D_H_ */
