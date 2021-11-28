/*
 * Gpio.h
 *
 *  Created on: 27 nov. 2021
 *      Author: Théo Magne
 */

#ifndef GPIO_H_
#define GPIO_H_


#include "../Inc/gpio.h"
#include "../Task_Manager/macro_types.h"

/*
 * @brief GPIO Set and Reset enumeration definition
 */
typedef enum GPIO_state_e
{
    GPIO_RESET = 0,
    GPIO_SET
} GPIO_state_e;

void GPIO_Write_Pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_state_e state);
GPIO_state_e GPIO_Read_Pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif /* GPIO_H_ */
