/*
 * Controller.c
 *
 *  Created on: 24 nov. 2021
 *      Author: Théo Magne
 */

#include "Controller.h"
#include "../Complementary_Filter/Complementary_Filter.h"
#include "../Sensors/Mpu.h"
#include "../System/Orientation.h"

#define KD_FILTER_COEF (0.5f)	/* Applied to the previous value */
#define KD_SPEED_FILTER_COEF (0.9f)	/* Applied to the previous value */

/* Private function prototypes */
static inline void CONTROLLER_Process(void);
static inline void CONTROLLER_process_velocity(void);
static inline void CONTROLLER_process_angle(void);

static const controller_config_t default_controller_config =
{
		/* Pid Configuration 	 Roll 		Pitch 		Yaw */
		.angle_kp = 		{	6.0f, 		7.0f, 		0.0f},
		.angle_kd = 		{	0.6f, 		0.7f, 		0.0f},
		.angle_speed_kp = 	{	10.0f, 		12.0f, 		40.0f},
		.angle_speed_ki = 	{	0.0f, 		0.0f, 		0.0f},
		.angle_speed_kd = 	{	0.0f, 		0.0f, 		0.0f}
};

static controller_t controller = { 0 };

void CONTROLLER_Init(void)
{
	/* Load configuration */
	controller.config = default_controller_config;
	controller.angle = COMPLEMENTARY_FILTER_Get_Angles();
	controller.angle_speed = ORIENTATION_Get_State_Vector();
	/* Initialize controller's state to "disabled" by default */
	controller.state = controller_state_eDISABLED;
	/* Copy paste pid settings from configuration */
	for(uint8_t a = 0; a < 3; a++)
	{
		controller.angle_speed_kp[a] = controller.config.angle_speed_kp[a];
		controller.angle_speed_ki[a] = controller.config.angle_speed_ki[a];
		controller.angle_speed_kd[a] = controller.config.angle_speed_kd[a];
		controller.angle_kp[a] = controller.config.angle_kp[a];
		controller.angle_kd[a] = controller.config.angle_kd[a];
	}
	/* Adapt I  terms to frequency */
	float period = MPU_Get_Period();
	for(uint8_t a = 0; a < 3; a++)
	{
		controller.angle_speed_ki[a] *= period;
		controller.angle_speed_kd[a] /= period;
		controller.angle_kd[a] /= period;
	}
}

void CONTROLLER_Process_Gyro(void)
{
	/* We don t run the "gyro process" if we are in simulation */
	if(controller.state == controller_state_eENABLE_P_GYRO)
	{
		CONTROLLER_Process();
	}
}

void CONTROLLER_Process_ms(void)
{
	/* If we are not in simulation mode, we run in the "process gyro" */
	if(controller.state == controller_state_eENABLE_P_MS)
	{
		CONTROLLER_Process();
	}
}

/*
 * @brief set the global thrust
 * @param thrust New thrust value
 */
void CONTROLLER_Set_Thrust(float thrust)
{
	controller.global_thrust = thrust;
}

void CONTROLLER_Set_Mode(controller_mode_e new_mode)
{
	controller.mode = new_mode;
}

void CONTROLLER_Set_State(controller_state_e new_state)
{
	controller.state = new_state;
}

float * CONTROLLER_Get_Angle_Target(void)
{
	return controller.target_angle;
}

float * CONTROLLER_Get_Angle_Speed_Target(void)
{
	return controller.target_angle_speed;
}

float * CONTROLLER_Get_Pid_Output(void)
{
	return controller.output_pid;
}

float * CONTROLLER_Get_Pid_Output_P(void)
{
	return controller.angle_speed_P;
}

float * CONTROLLER_Get_Pid_Output_I(void)
{
	return controller.angle_speed_I;
}

float * CONTROLLER_Get_Pid_KP(void)
{
	return controller.angle_speed_kp;
}
float * CONTROLLER_Get_Pid_KI(void)
{
	return controller.angle_speed_ki;
}

/* Static functions definition */


/*
 * @brief Controller compute pid
 */
static inline void CONTROLLER_Process()
{
	if(controller.mode == controller_mode_eANGLE)
	{
		CONTROLLER_process_angle();
	}
	CONTROLLER_process_velocity();

	/* Mix each pid output */
	controller.output_motor[orien_control_vector_eMOTOR_FL] = 	controller.global_thrust + controller.output_pid[axe_eROLL] - controller.output_pid[axe_ePITCH] - controller.output_pid[axe_eYAW];
	controller.output_motor[orien_control_vector_eMOTOR_FR] = 	controller.global_thrust - controller.output_pid[axe_eROLL] - controller.output_pid[axe_ePITCH] + controller.output_pid[axe_eYAW];
	controller.output_motor[orien_control_vector_eMOTOR_BR] = 	controller.global_thrust - controller.output_pid[axe_eROLL] + controller.output_pid[axe_ePITCH] - controller.output_pid[axe_eYAW];
	controller.output_motor[orien_control_vector_eMOTOR_BL] = 	controller.global_thrust + controller.output_pid[axe_eROLL] + controller.output_pid[axe_ePITCH] + controller.output_pid[axe_eYAW];
	MOTOR_Set(controller.output_motor);
}

/*
 * @brief compute angle velocity error and correction using a pid controller
 */
static inline void CONTROLLER_process_velocity(void)
{
	/* First thing first, errors */
	controller.angle_speed_error[axe_eROLL] = controller.target_angle_speed[axe_eROLL] - controller.angle_speed[axe_eROLL];
	controller.angle_speed_error[axe_ePITCH] = controller.target_angle_speed[axe_ePITCH] - controller.angle_speed[axe_ePITCH];
	controller.angle_speed_error[axe_eYAW] = controller.target_angle_speed[axe_eYAW] - controller.angle_speed[axe_eYAW];

	/* Now, PID's time ! */
	/* P */
	controller.angle_speed_P[axe_eROLL] = controller.angle_speed_error[axe_eROLL] * controller.angle_speed_kp[axe_eROLL];
	controller.angle_speed_P[axe_ePITCH] = controller.angle_speed_error[axe_ePITCH] * controller.angle_speed_kp[axe_ePITCH];
	controller.angle_speed_P[axe_eYAW] = controller.angle_speed_error[axe_eYAW] * controller.angle_speed_kp[axe_eYAW];
	/* I */
	controller.angle_speed_I[axe_eROLL] += controller.angle_speed_error[axe_eROLL] * controller.angle_speed_ki[axe_eROLL];
	controller.angle_speed_I[axe_ePITCH] += controller.angle_speed_error[axe_ePITCH] * controller.angle_speed_ki[axe_ePITCH];
	controller.angle_speed_I[axe_eYAW] += controller.angle_speed_error[axe_eYAW] * controller.angle_speed_ki[axe_eYAW];
	/* D */
	float d_roll = controller.angle_speed[axe_eROLL] - controller.angle_speed_previous[axe_eROLL];
	d_roll *= controller.angle_speed_kd[axe_eROLL];
	controller.angle_speed_D[axe_eROLL] = KD_SPEED_FILTER_COEF * controller.angle_speed_D[axe_eROLL] + (1 - KD_SPEED_FILTER_COEF) * d_roll;


	float d_pitch = controller.angle_speed[axe_ePITCH] - controller.angle_speed_previous[axe_ePITCH];
	d_pitch *= controller.angle_speed_kd[axe_ePITCH];
	controller.angle_speed_D[axe_ePITCH] = KD_SPEED_FILTER_COEF * controller.angle_speed_D[axe_ePITCH] + (1 - KD_SPEED_FILTER_COEF) * d_pitch;

	/* Add I, D and P terms */
	controller.output_pid[axe_eROLL] = controller.angle_speed_P[axe_eROLL] + controller.angle_speed_I[axe_eROLL] - controller.angle_speed_D[axe_eROLL];
	controller.output_pid[axe_ePITCH] = controller.angle_speed_P[axe_ePITCH] + controller.angle_speed_I[axe_ePITCH] - controller.angle_speed_D[axe_ePITCH];
	controller.output_pid[axe_eYAW] = controller.angle_speed_P[axe_eYAW] + controller.angle_speed_I[axe_eYAW] - controller.angle_speed_D[axe_eYAW];
}

/*
 * @brief compute angle errors and correction using a pid controller
 */
static inline void CONTROLLER_process_angle(void)
{
	/* First thing first, errors */
	controller.angle_error[axe_eROLL] = controller.target_angle[axe_eROLL] - controller.angle[axe_eROLL];
	controller.angle_error[axe_ePITCH] = controller.target_angle[axe_ePITCH] - controller.angle[axe_ePITCH];
	/* Now, PID's time ! */
	/* P */
	controller.angle_P[axe_eROLL] = controller.angle_error[axe_eROLL] * controller.angle_kp[axe_eROLL];
	controller.angle_P[axe_ePITCH] = controller.angle_error[axe_ePITCH] * controller.angle_kp[axe_ePITCH];
	/* D */
	float d_roll = controller.angle[axe_eROLL] - controller.angle_previous[axe_eROLL];
	d_roll *= controller.angle_kd[axe_eROLL];
	controller.angle_D[axe_eROLL] = KD_FILTER_COEF * controller.angle_D[axe_eROLL] + (1 - KD_FILTER_COEF) * d_roll;


	float d_pitch = controller.angle[axe_ePITCH] - controller.angle_previous[axe_ePITCH];
	d_pitch *= controller.angle_kd[axe_ePITCH];
	controller.angle_D[axe_ePITCH] = KD_FILTER_COEF * controller.angle_D[axe_ePITCH] + (1 - KD_FILTER_COEF) * d_pitch;

	controller.angle_previous[axe_eROLL] = controller.angle[axe_eROLL];
	controller.angle_previous[axe_ePITCH] = controller.angle[axe_ePITCH];

	/* Add I and P terms */
	controller.target_angle_speed[axe_eROLL] = controller.angle_P[axe_eROLL] - controller.angle_D[axe_eROLL];
	controller.target_angle_speed[axe_ePITCH] = controller.angle_P[axe_ePITCH] - controller.angle_D[axe_ePITCH];
	controller.target_angle_speed[axe_eYAW] = controller.target_angle[axe_eYAW];


}





