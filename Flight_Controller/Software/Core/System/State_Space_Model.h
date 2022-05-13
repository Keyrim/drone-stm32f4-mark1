/*
 * State_Space_Model.h
 *
 *  Created on: 2 juin 2021
 *      Author: Théo
 */

#ifndef STATE_SPACE_MODEL_STATE_SPACE_MODEL_H_
#define STATE_SPACE_MODEL_STATE_SPACE_MODEL_H_

#include "../../Drivers/CMSIS/DSP/Include/arm_math.h"
#include "stdint.h"

#define STATE_VECTOR_MAX_SIZE 	6	//n
#define INPUT_MAX_SIZE 			4	//m
#define OUTPUT_MAX_SIZE 		6	//l

/*
 * @brief
 * x_size n
 * u_size m
 * z_size l
 * F system's dynamic matrix
 * B control matrix
 * H Observation matrix
 */

#define ABSTRACT_STATE_SPACE_MODEL_T(state_vector_size_, input_size_, output_size_) \
	uint8_t x_size ; \
	uint8_t u_size ; \
	uint8_t z_size ; \
	\
	arm_matrix_instance_f32 * F; \
	arm_matrix_instance_f32 F_trans; \
	float F_trans_array[state_vector_size_ * state_vector_size_]; \
	\
	arm_matrix_instance_f32 * B; \
	\
	arm_matrix_instance_f32 * H; \
	arm_matrix_instance_f32 H_trans; \
	float H_trans_array[state_vector_size_ * output_size_]; \
	\
	arm_matrix_instance_f32 x_tmp; \
	float x_tmp_array [state_vector_size_]; \
	\
	arm_matrix_instance_f32 x; \
	float x_array [state_vector_size_]; \
	arm_matrix_instance_f32 x_ref; \
	float x_ref_array[state_vector_size_]; \
	arm_matrix_instance_f32 u; \
	float u_array [input_size_]; \
	arm_matrix_instance_f32 z; \
	float z_array [output_size_]; \

typedef struct State_Space_Model_t
{
	ABSTRACT_STATE_SPACE_MODEL_T(STATE_VECTOR_MAX_SIZE, INPUT_MAX_SIZE, OUTPUT_MAX_SIZE);
}State_Space_Model_t;

void STATE_SPACE_MODEL_Init(State_Space_Model_t * ss_model, arm_matrix_instance_f32 * F,arm_matrix_instance_f32 * B, arm_matrix_instance_f32 * H);
void STATE_SPACE_MODEL_Set_x0(State_Space_Model_t * ss_model, float * x_values);
void STATE_SPACE_MODEL_Step(State_Space_Model_t * ss_model);


#endif /* STATE_SPACE_MODEL_STATE_SPACE_MODEL_H_ */
