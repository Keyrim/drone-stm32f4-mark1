/*
 * Complementary_Filter.c
 *
 *  Created on: 23 nov. 2021
 *      Author: Théo Magne
 */

#include "Complementary_Filter.h"
#include "../Sensors/Mpu.h"
#include "math.h"
#include "../../Drivers/CMSIS/DSP/Include/arm_math.h"

#define RAD_TO_DEG ((float)57.2957795f)
#define DEG_TO_RAD ((float)0.01745329f)

#define FREQUENCY	1000
#define ALPHA		((float)0.998f)

static complementary_filter_t filter =
{
		.frequency = FREQUENCY,
		.period = (1.0f / (float)FREQUENCY)
};

/* Static function prototype */
static void angle_180(float * x);

void COMPLEMENTARY_FILTER_Init(void)
{
	filter.gyro = MPU_Get_Gyro_Ptr();
	filter.gyro_raw = NULL;
	filter.acc = MPU_Get_Acc_Ptr();
	filter.alpha_gyro = ALPHA;
	filter.alpha_acc = 1.0f - ALPHA;
}

void COMPLEMENTARY_FILTER_Process(void)
{
	/* Total acceleration */
	float acc_total = filter.acc[0] * filter.acc[0];
	acc_total += filter.acc[1] * filter.acc[1];
	acc_total += filter.acc[2] * filter.acc[2];
	acc_total = sqrtf(acc_total);

	/* Guess the angles according to the accelerometer and some trigonometry */
	if(acc_total)
	{
		float inv_acc_total = 1.0f / acc_total;
		if(absolute(filter.acc[axe_eROLL]) < acc_total)
		{
			filter.angle_acc[axe_ePITCH] = 	-asinf(filter.acc[axe_eROLL] * inv_acc_total) * RAD_TO_DEG;
		}
		if(absolute(filter.acc[axe_ePITCH]) < acc_total)
		{
			filter.angle_acc[axe_eROLL] = 	asinf(filter.acc[axe_ePITCH] * inv_acc_total) * RAD_TO_DEG;
		}
	}

	if(!filter.used_once)
	{
		filter.used_once = TRUE;
		filter.angle[axe_eROLL] = filter.angle_acc[axe_eROLL];
		filter.angle[axe_ePITCH] = filter.angle_acc[axe_ePITCH];
		filter.angle[axe_eYAW] = 0;
	}
	else
	{
		/* Distance in degree since the last epoch */
		float dx = filter.gyro[axe_eROLL] * filter.period;
		float dy = filter.gyro[axe_ePITCH] * filter.period;
		float dz = filter.gyro[axe_eYAW] * filter.period;
		/* Update angles according to distance travelled on each axis */
		filter.angle[axe_eROLL] += dx;
		filter.angle[axe_ePITCH] += dy;
		filter.angle[axe_eYAW] += dz;
		/* Yaw drift compensation */
		float sin_dz = sinf(DEG_TO_RAD * dz);
		filter.angle[axe_eROLL] += sin_dz * filter.angle[axe_ePITCH];
		filter.angle[axe_ePITCH] -= sin_dz * filter.angle[axe_eROLL];
		/* We use this to make sure our angles doesnt go above PI */
		angle_180(&filter.angle[axe_eROLL]);
		angle_180(&filter.angle[axe_ePITCH]);
		/* Finally, the complementary filter */
		filter.angle[axe_eROLL] = filter.alpha_gyro * filter.angle[axe_eROLL] + filter.alpha_acc * filter.angle_acc[axe_eROLL];
		filter.angle[axe_ePITCH] = filter.alpha_gyro * filter.angle[axe_ePITCH] + filter.alpha_acc * filter.angle_acc[axe_ePITCH];
	}

}

float * COMPLEMENTARY_FILTER_Get_Angles(void)
{
	return filter.angle;
}

float * COMPLEMENTARY_FILTER_Get_Angles_Acc(void)
{
	return filter.angle_acc;
}

static void angle_180(float * x)
{
	if(*x > 180)
	{
		*x -= 360;
	}
	else if( *x < -180)
	{
		*x += 360 ;
	}
}
