/*
 * State_Space_Model.c
 *
 *  Created on: 2 juin 2021
 *      Author: Théo
 */


#include "State_Space_Model.h"

State_Space_Model_t ss;


void STATE_SPACE_MODEL_Init(State_Space_Model_t * ss_model, arm_matrix_instance_f32 * F,arm_matrix_instance_f32 * B, arm_matrix_instance_f32 * H)
{
	//Dimensions
	ss_model->x_size = F->numCols;
	ss_model->u_size = B->numCols;
	ss_model->z_size = H->numRows;

	//State space matricies init
	ss_model->F = F;
	arm_mat_init_f32(&ss_model->F_trans, ss_model->x_size, ss_model->x_size, ss_model->F_trans_array);
	arm_mat_trans_f32(ss_model->F, &ss_model->F_trans);

	ss_model->B = B;
	ss_model->H = H;

	//H transposed init and calcul
//	arm_mat_init_f32(&ss_model->H_trans, H->numCols, H->numRows, ss_model->H_trans_array);
//	arm_mat_trans_f32(H, &ss_model->H_trans);

	//x, x_ref & x_tmp vector init
	arm_mat_init_f32(&ss_model->x, ss_model->x_size, 1, ss_model->x_array);
	arm_mat_init_f32(&ss_model->x_tmp, ss_model->x_size, 1, ss_model->x_tmp_array);
	arm_mat_init_f32(&ss_model->x_ref, ss_model->x_size, 1, ss_model->x_ref_array);


	//u vector init
	arm_mat_init_f32(&ss_model->u, ss_model->u_size, 1, ss_model->u_array);

	//z vector init
	arm_mat_init_f32(&ss_model->z, ss_model->z_size, 1, ss_model->z_array);
}

void STATE_SPACE_MODEL_Set_x0(State_Space_Model_t * ss_model, float * x_values)
{
	for(uint8_t x = 0; x < ss_model->x_size; x++)
		ss_model->x_array[x] = x_values[x];
}

void STATE_SPACE_MODEL_Step(State_Space_Model_t * ss_model)
{
	//Predict next state
	arm_mat_mult_f32(ss_model->B, &ss_model->u, &ss_model->x_tmp);
	arm_mat_mult_f32(ss_model->F, &ss_model->x, &ss_model->x);
	arm_mat_add_f32(&ss_model->x_tmp, &ss_model->x, &ss_model->x);
}

