/*
 * Orientation.h
 *
 *  Created on: 8 janv. 2022
 *      Author: Théo Magne
 */

#ifndef SYSTEM_ORIENTATION_H_
#define SYSTEM_ORIENTATION_H_

#include "State_Space_Model.h"

//typedef enum
//{
//	orientation_eROLL = 0,
//	orientation_e
//	orientation_ePITCH,
//	orientation_eYAW,
//	orientation_eROLL_RATE,
//}orientation_state_vector_e;

typedef struct
{
	ABSTRACT_STATE_SPACE_MODEL_T()
}orientation_model_t;

#endif /* SYSTEM_ORIENTATION_H_ */
