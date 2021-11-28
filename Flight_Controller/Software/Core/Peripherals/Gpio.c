/*
 * Gpio.c
 *
 *  Created on: 27 nov. 2021
 *      Author: Théo
 */

#include "Gpio.h"

/*
 * @brief Write the state of a gpio pin
 * @param GPIOx pointer to an hal gpio handler structure
 * @param GPIO_Pin Number of the pin
 * @param state State to write to the pin
 */
void GPIO_Write_Pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_state_e state)
{
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, state);
}

/*
 * @brief Read the state of a gpio pin
 * @param GPIOx pointer to an hal gpio handler structure
 * @param GPIO_Pin Number of the pin
 * @return The state of the pin
 */
GPIO_state_e GPIO_Read_Pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    return HAL_GPIO_ReadPin(GPIOx, GPIO_Pin);
}
