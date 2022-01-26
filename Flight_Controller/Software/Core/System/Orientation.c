/*
 * Orientation.c
 *
 *  Created on: 17 janv. 2022
 *      Author: Théo Magne
 */


#include "Orientation.h"

orientation_model_t orientation;
/* --------------- State Space Model matrices --------------- */
/* F Matrix definition */
static arm_matrix_instance_f32 F = {0};
static float F_array[orien_state_vector_eCOUNT * orien_state_vector_eCOUNT] =
{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
};

/* B Matrix definition TODO compute the true value (constant at least) */
static arm_matrix_instance_f32 B = {0};
static float B_array[orien_state_vector_eCOUNT * orien_control_vector_eCOUNT] =
{
	0, 	0, 	0, 	0,
	0, 	0,	0,	0,
	0,	0,	0,	0,
};

/* Observation Matrix definition */
static arm_matrix_instance_f32 H = {0};
static float H_array[orien_meas_vector_eCOUNT * orien_state_vector_eCOUNT] =
{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1
};

/* --------------- State Space Model matrices --------------- */
/* Measurement covariance matrix */
static arm_matrix_instance_f32 R ;
static float R_array[orien_meas_vector_eCOUNT*orien_meas_vector_eCOUNT] =
{
		100, 0, 0,
		0, 100, 0,
		0, 0, 100
};
/* system model covariance matrix */
static arm_matrix_instance_f32 Q ;
static float Q_array[orien_state_vector_eCOUNT*orien_state_vector_eCOUNT] =
{
		0.01, 	0, 		0,
		0,		0.01, 	0,
		0,		0,		0.01
};
/* P Matrix definition */
static arm_matrix_instance_f32 P_predict ;
static float P_array[orien_state_vector_eCOUNT*orien_state_vector_eCOUNT] =
{
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
};

void ORIENTATION_Init(void)
{
	/* State space model initialization */
	arm_mat_init_f32(&F, orien_state_vector_eCOUNT, orien_state_vector_eCOUNT, F_array);
	arm_mat_init_f32(&B, orien_state_vector_eCOUNT, orien_control_vector_eCOUNT, B_array);
	arm_mat_init_f32(&H, orien_meas_vector_eCOUNT, orien_state_vector_eCOUNT, H_array);
	STATE_SPACE_MODEL_Init((State_Space_Model_t*)&orientation, &F, &B, &H);
	/* Kalman Model initialization */
	arm_mat_init_f32(&R, orien_meas_vector_eCOUNT, orien_meas_vector_eCOUNT, R_array);
	arm_mat_init_f32(&Q, orien_state_vector_eCOUNT, orien_state_vector_eCOUNT, Q_array);
	arm_mat_init_f32(&P_predict, orien_state_vector_eCOUNT, orien_state_vector_eCOUNT, P_array);
	KALMAN_Init(&orientation.kalman, (State_Space_Model_t*)&orientation, &P_predict, &Q, &R);

}

void ORIENTATION_Update(void)
{
	KALMAN_Update(&orientation.kalman);
	KALMAN_Predict(&orientation.kalman);

}
