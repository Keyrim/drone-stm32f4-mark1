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

/* Private function prototypes */
static inline void CONTROLLER_Process(void);

static const controller_config_t default_controller_config =
{
		/* Pid Configuration 	 Roll 		Pitch 		Yaw */
		.angle_kp = 		{	4.0f, 		4.0f, 		0},
		.angle_speep_kp = 	{	6.0f, 		0.8f, 		1.2f}
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

/* Static functions definition */


/*
 * @brief Controller compute pid
 */
static inline void CONTROLLER_Process()
{
	/* First thing first, errors */
	controller.angle_speed_error[axe_eROLL] = controller.target_angle_speed[axe_eROLL] - controller.angle_speed[axe_eROLL];
	controller.angle_speed_error[axe_ePITCH] = 0; //controller.target_angle_speed[axe_ePITCH] - controller.angle_speed[axe_ePITCH];
	controller.angle_speed_error[axe_eYAW] = 0; //controller.target_angle_speed[axe_eYAW] - controller.angle_speed[axe_eYAW];

	/* Now, PID's time ! */
	controller.output_pid[axe_eROLL] = controller.angle_speed_error[axe_eROLL] * controller.config.angle_speep_kp[axe_eROLL];
	controller.output_pid[axe_ePITCH] = controller.angle_speed_error[axe_ePITCH] * controller.config.angle_speep_kp[axe_ePITCH];
	controller.output_pid[axe_eYAW] = controller.angle_speed_error[axe_eYAW] * controller.config.angle_speep_kp[axe_eYAW];
	/* Mix each pid output */
	controller.output_motor[orien_control_vector_eMOTOR_FL] = 	controller.global_thrust + controller.output_pid[axe_eROLL] - controller.output_pid[axe_ePITCH] - controller.output_pid[axe_eYAW];
	controller.output_motor[orien_control_vector_eMOTOR_FR] = 	controller.global_thrust - controller.output_pid[axe_eROLL] - controller.output_pid[axe_ePITCH] + controller.output_pid[axe_eYAW];
	controller.output_motor[orien_control_vector_eMOTOR_BR] = 	controller.global_thrust - controller.output_pid[axe_eROLL] + controller.output_pid[axe_ePITCH] - controller.output_pid[axe_eYAW];
	controller.output_motor[orien_control_vector_eMOTOR_BL] = 	controller.global_thrust + controller.output_pid[axe_eROLL] + controller.output_pid[axe_ePITCH] + controller.output_pid[axe_eYAW];
	MOTOR_Set(controller.output_motor);
}











