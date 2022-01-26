/*
 * Kalman.h
 *
 *  Created on: 10 mai 2021
 *      Author: Théo
 */

#ifndef KALMAN_H_
#define KALMAN_H_

#include "../../Drivers/CMSIS/DSP/Include/arm_math.h"
#include "State_Space_Model.h"

typedef enum kalman_state_e{
	KALMAN_NOT_INIT,
	KALMAN_INIT_FAILED,
	KALMAN_OK
}kalman_state_e;

typedef struct kalman_t{

	State_Space_Model_t * ss ;

	arm_matrix_instance_f32 x_predict_tmp;
	float x_predict_tmp_array [STATE_VECTOR_MAX_SIZE];

	arm_matrix_instance_f32 x_predict;
	float x_predict_array [STATE_VECTOR_MAX_SIZE];

	arm_matrix_instance_f32 inovation;
	float inovation_array[OUTPUT_MAX_SIZE];

	arm_matrix_instance_f32 P ;	//Covariance estimation
	float P_array[STATE_VECTOR_MAX_SIZE * STATE_VECTOR_MAX_SIZE];
	arm_matrix_instance_f32 * P_predict;


	arm_matrix_instance_f32 * Q;	//Covariance système (process noise)

	arm_matrix_instance_f32 * R;	//Covariance mesure

	arm_matrix_instance_f32 S;	//Covariance innovation
	float S_array[OUTPUT_MAX_SIZE * OUTPUT_MAX_SIZE];

	arm_matrix_instance_f32 S_inversed;	//Covariance innovation
	float S_inversed_array[OUTPUT_MAX_SIZE * OUTPUT_MAX_SIZE];

	arm_matrix_instance_f32 S_tmp;	//Calcul intérmédiaire cov. inovation
	float S_tmp_array[STATE_VECTOR_MAX_SIZE * OUTPUT_MAX_SIZE];

	arm_matrix_instance_f32 K;	//Kalman gain
	float K_array[STATE_VECTOR_MAX_SIZE * OUTPUT_MAX_SIZE];

	arm_matrix_instance_f32 I_n ;	//Identité de taille n
	float I_n_array[STATE_VECTOR_MAX_SIZE*STATE_VECTOR_MAX_SIZE] ;

	kalman_state_e state;
	arm_status arm_result;

}kalman_t;

kalman_state_e KALMAN_Init(kalman_t *kalman, State_Space_Model_t * ss ,arm_matrix_instance_f32 * P_predict, arm_matrix_instance_f32 * Q, arm_matrix_instance_f32 * R);
kalman_state_e KALMAN_Update(kalman_t * kalman);
kalman_state_e KALMAN_Predict(kalman_t * kalman);

#endif /* KALMAN_H_ */
