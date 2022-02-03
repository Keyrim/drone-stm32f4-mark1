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

/*
 * @brief abstract kalman model
 * x_predict_tmp state_vector_size
 * x_predict state_vector_size
 * innovation output_size
 * P state_vector_size * state_vector_size
 * S output_size * output_size
 * S_inverse output_size * output_size
 * S_tmp output_size * state_vector_size
 * K state_vector_size * output_size
 * I state_vector_size * state_vector_size
 */
#define ABSTRACT_KALMAN_MODEL_T(state_vector_size_, output_size_) \
		State_Space_Model_t * ss ;	\
		\
		arm_matrix_instance_f32 x_predict_tmp;	\
		float x_predict_tmp_array [state_vector_size_];	\
		\
		arm_matrix_instance_f32 x_predict;	\
		float x_predict_array [state_vector_size_]; \
		\
		arm_matrix_instance_f32 inovation; \
		float inovation_array[output_size_]; \
		\
		arm_matrix_instance_f32 P ;	\
		float P_array[state_vector_size_ * state_vector_size_]; \
		arm_matrix_instance_f32 * P_predict;	\
		\
		arm_matrix_instance_f32 * Q;	\
		\
		arm_matrix_instance_f32 * R;	\
		\
		arm_matrix_instance_f32 S;	\
		float S_array[output_size_ * output_size_];	\
		\
		arm_matrix_instance_f32 S_inversed;	\
		float S_inversed_array[output_size_ * output_size_];	\
		\
		arm_matrix_instance_f32 S_tmp;	\
		float S_tmp_array[state_vector_size_ * output_size_];	\
		\
		arm_matrix_instance_f32 K;	\
		float K_array[state_vector_size_ * output_size_];	\
		\
		arm_matrix_instance_f32 I_n;	\
		float I_n_array[state_vector_size_ * state_vector_size_];	\
		\
		kalman_state_e state;	\
		arm_status arm_result;	\


typedef struct kalman_t
{
	ABSTRACT_KALMAN_MODEL_T(STATE_VECTOR_MAX_SIZE,OUTPUT_MAX_SIZE);
}kalman_t;

kalman_state_e KALMAN_Init(kalman_t *kalman, State_Space_Model_t * ss ,arm_matrix_instance_f32 * P_predict, arm_matrix_instance_f32 * Q, arm_matrix_instance_f32 * R);
kalman_state_e KALMAN_Update(kalman_t * kalman);
kalman_state_e KALMAN_Predict(kalman_t * kalman);

#endif /* KALMAN_H_ */
