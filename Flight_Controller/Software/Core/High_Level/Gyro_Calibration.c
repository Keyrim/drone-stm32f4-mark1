/*
 * Gyro_Calibration.c
 *
 *  Created on: 11 juin 2022
 *      Author: Théo Magne
 */


#include "stm32f4xx_hal.h"
#include "High_Level.h"
#include "../Sensors/Mpu.h"


#define NUMBER_OF_VALUE	(1000)
#define DIVIDER ((float)(1.0f/NUMBER_OF_VALUE))

static uint16_t counter = 0;
static float calibration_value[axe_eCOUNT] = { 0 };
static float * gyro;

void GYRO_CALIBRATION_Init(high_level_t * high_level)
{
	counter = 0;
	calibration_value[0] = 0;
	calibration_value[1] = 0;
	calibration_value[2] = 0;
	gyro = MPU_Get_Gyro_Ptr();
}

void GYRO_CALIBRATION_Main(high_level_t * high_level)
{
	if(counter == NUMBER_OF_VALUE)
	{
		calibration_value[0] *= DIVIDER;
		calibration_value[1] *= DIVIDER;
		calibration_value[2] *= DIVIDER;
		MPU_Set_Gyro_Calibration(calibration_value);
		high_level->state = high_level_eIDLE;
	}
	else
	{
		calibration_value[0] += gyro[0];
		calibration_value[1] += gyro[1];
		calibration_value[2] += gyro[2];
		counter ++;
	}
}
