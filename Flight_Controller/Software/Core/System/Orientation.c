/*
 * Orientation.c
 *
 *  Created on: 17 janv. 2022
 *      Author: Théo Magne
 */


#include "Orientation.h"

/* Private functions */
static void angle_from_acc(float * acc, float * angles);

orientation_model_t orientation;



void Orientation_Init(void)
{

}

/*
 * @brief Calculate angles from accelerometer
 */
float void angle_from_acc(float * acc, float * angles)
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
}
