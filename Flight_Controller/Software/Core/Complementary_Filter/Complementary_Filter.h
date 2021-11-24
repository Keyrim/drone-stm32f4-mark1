/*
 * Complementary_Filter.h
 *
 *  Created on: 23 nov. 2021
 *      Author: Théo MAGNE
 */

#ifndef COMPLEMENTARY_FILTER_H_
#define COMPLEMENTARY_FILTER_H_

#include "stdint.h"
#include "../Task_Manager/macro_types.h"



typedef struct
{
	float * gyro;					/* [degrees / s] 		Filtered gyro data */
	float * gyro_raw;				/* [degrees / s] 		Raw gyro data */
	float * acc;					/* [g] 					Raw acc data */
	float angle[axe_eCOUNT];		/* [degree]	 			Angular position */
	float angle_acc[axe_eCOUNT];	/* [degree]				Angular position obtained from acc data */

	uint32_t frequency;				/* [Hz] 				Frequency at which we call the filter */
	float period;					/* [s] */

	float alpha_gyro;
	float alpha_acc;

	bool_e used_once;	/*						false at startup, true once process has been called once */
}complementary_filter_t;

void COMPLEMENTARY_FILTER_Init(void);
void COMPLEMENTARY_FILTER_Process(void);
complementary_filter_t * COMPLEMENTARY_FILTER_Get_Filter(void);

#endif /* COMPLEMENTARY_FILTER_H_ */
