/*
 * Orientation.h
 *
 *  Created on: 8 janv. 2022
 *      Author: Théo Magne
 */

#ifndef SYSTEM_ORIENTATION_H_
#define SYSTEM_ORIENTATION_H_

#include "State_Space_Model.h"
#include "Kalman.h"

/*
 * @brief orientation state vector enumeration definition
 */
typedef enum
{
	orien_state_vector_eVELOCITY_ROLL = 0,
	orien_state_vector_eVELOCITY_PITCH,
	orien_state_vector_eVELOCITY_YAW,
	orien_state_vector_eCOUNT
}orientation_state_vector_e;


/*
 * @brief orientation control vector enumeration definition
 */
typedef enum
{
	orien_control_vector_eMOTOR_FL = 0,
	orien_control_vector_eMOTOR_FR,
	orien_control_vector_eMOTOR_BR,
	orien_control_vector_eMOTOR_BL,
	orien_control_vector_eCOUNT
}orientation_control_vector_e;

/*
 * @brief orientation measurement vector enumeration definition
 */
typedef enum
{
	orien_meas_vector_eVELOCITY_ROLL = 0,
	orien_meas_vector_eVELOCITY_PITCH,
	orien_meas_vector_eVELOCITY_YAW,
	orien_meas_vector_eCOUNT,
}orientation_measurement_vector_e;

/*
 * @brief motor configuration structure definition
 */
typedef struct
{
	float x;
	float y;
}motor_config_t;

/*
 * @brief Orientation configuration structure definition
 */
typedef struct
{
	uint8_t prescaler;	/* Prescaler relatively to the gyroscope update frequency */
	motor_config_t motor[4];	/* Motors positions */
	float inertia_matrix[9];	/* Moment of inertia matrix */
}orientation_config_t;

/*
 * @brief Orientation kalman structure inherited from kalman abstract structure
 */
typedef struct
{
	ABSTRACT_KALMAN_MODEL_T(orien_state_vector_eCOUNT,
							orien_meas_vector_eCOUNT);
}orientation_kalman_t;

/*
 * @brief Orientation kalman structure inherited from kalman abstract structure
 */
typedef struct
{
	ABSTRACT_STATE_SPACE_MODEL_T(orien_state_vector_eCOUNT,
								 orien_control_vector_eCOUNT,
								 orien_meas_vector_eCOUNT);
	orientation_config_t config;
	float period;	/* update period in seconds */
}orientation_model_t;

void ORIENTATION_Init(void);

#endif /* SYSTEM_ORIENTATION_H_ */
