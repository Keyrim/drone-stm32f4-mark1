/*
 * Kalman.c
 *
 *  Created on: 10 mai 2021
 *      Author: Théo
 */

#include "Kalman.h"


kalman_state_e KALMAN_Init(kalman_t *kalman, State_Space_Model_t * ss ,arm_matrix_instance_f32 * P_predict, arm_matrix_instance_f32 * Q, arm_matrix_instance_f32 * R){

	kalman->ss = ss;
	//Covariance
	kalman->Q = Q;
	kalman->R = R;
	kalman->P_predict = P_predict;

	//X predict
	arm_mat_init_f32(&kalman->x_predict, ss->x_size, 1, kalman->x_predict_array);
	for(uint8_t i =0; i > ss->x_size; i++)
		kalman->x_predict.pData[i] = ss->x.pData[i];

	//X predict tmp
	arm_mat_init_f32(&kalman->x_predict_tmp, ss->x_size, 1, kalman->x_predict_tmp_array);

	arm_mat_init_f32(&kalman->inovation, ss->z_size, 1, kalman->inovation_array);
	arm_mat_init_f32(&kalman->S, ss->z_size, ss->z_size, kalman->S_array);
	arm_mat_init_f32(&kalman->S_inversed, ss->z_size, ss->z_size, kalman->S_inversed_array);
	arm_mat_init_f32(&kalman->S_tmp, ss->z_size, ss->x_size, kalman->S_tmp_array);
	arm_mat_init_f32(&kalman->P, ss->x_size, ss->x_size, kalman->P_array);

	//Kalman gain
	arm_mat_init_f32(&kalman->K, ss->x_size, ss->z_size, kalman->K_array);

	//Identity matrix
	arm_mat_init_f32(&kalman->I_n, ss->x_size, ss->x_size, kalman->I_n_array);
	for(uint8_t i = 0; i < ss->x_size; i++)
		kalman->I_n.pData[i+i*ss->x_size] = 1;

	kalman->state = KALMAN_OK;

	return KALMAN_OK ;
}

kalman_state_e KALMAN_Update(kalman_t * kalman){

	//	---------------	Update ----------------

	State_Space_Model_t * ss = kalman->ss ;


	//Inovation
	kalman->arm_result = arm_mat_mult_f32(ss->H, &kalman->x_predict, &kalman->inovation);
	kalman->arm_result = arm_mat_sub_f32(&ss->z, &kalman->inovation, &kalman->inovation);

	//Covariance innovation
	kalman->arm_result = arm_mat_mult_f32(ss->H, kalman->P_predict, &kalman->S_tmp);
	kalman->arm_result = arm_mat_mult_f32(&kalman->S_tmp, &ss->H_trans, &kalman->S);
	kalman->arm_result = arm_mat_add_f32(&kalman->S, kalman->R, &kalman->S);
	kalman->arm_result = arm_mat_inverse_f32(&kalman->S, &kalman->S_inversed);

	//Kalman gain
	kalman->arm_result = arm_mat_mult_f32(kalman->P_predict, &ss->H_trans, &kalman->K);
	kalman->arm_result = arm_mat_mult_f32(&kalman->K, &kalman->S_inversed, &kalman->K);

	//State update
	kalman->arm_result = arm_mat_mult_f32(&kalman->K, &kalman->inovation, &ss->x);
	kalman->arm_result = arm_mat_add_f32(&kalman->x_predict, &ss->x, &ss->x);

	//Covariance update
	kalman->arm_result = arm_mat_mult_f32(&kalman->K, kalman->ss->H, &kalman->P);
	kalman->arm_result = arm_mat_sub_f32(&kalman->I_n, &kalman->P, &kalman->P);
	kalman->arm_result = arm_mat_mult_f32(&kalman->P, kalman->P_predict, &kalman->P);

	return KALMAN_OK;
}

kalman_state_e KALMAN_Predict(kalman_t * kalman){

	//Predict next state
	arm_mat_mult_f32(kalman->ss->B, &kalman->ss->u, &kalman->x_predict_tmp);
	arm_mat_mult_f32(kalman->ss->F, &kalman->ss->x, &kalman->x_predict);
	arm_mat_add_f32(&kalman->x_predict_tmp, &kalman->x_predict, &kalman->x_predict);

	//Predict next estimation covariance
	arm_mat_mult_f32(kalman->ss->F, &kalman->P, kalman->P_predict);
	arm_mat_mult_f32(kalman->P_predict, &kalman->ss->F_trans, kalman->P_predict);
	arm_mat_add_f32(kalman->P_predict, kalman->Q, kalman->P_predict);

	return KALMAN_OK ;
}



















