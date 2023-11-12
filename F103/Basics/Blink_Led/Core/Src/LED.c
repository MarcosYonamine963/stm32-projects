/*
 * LED.c
 *
 *  Created on: Nov 12, 2023
 *      Author: MarcosYonamine963
 *
 *  Rev: 0
 */


#include "LED.h"

void LED_Init(LED_t *LED)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};


	  /* GPIO Ports Clock Enable */
	  switch(LED->GPIO_Port)
	  {
	  	  case (uint32_t)GPIOA:

	  		  __HAL_RCC_GPIOA_CLK_ENABLE();
	  		  break;

	  	  case (uint32_t)GPIOB:
	  		  __HAL_RCC_GPIOB_CLK_ENABLE();
	  		  break;

	  	  case (uint32_t)GPIOC:
	  		  __HAL_RCC_GPIOC_CLK_ENABLE();
	  		  break;

	  	  case (uint32_t)GPIOD:
	  		  __HAL_RCC_GPIOD_CLK_ENABLE();
	  		  break;

	  	  default:
	  		  break;
	  } // end switch(LED->GPIO_Port)

	  /*Configure GPIO pin Output Level */
	  HAL_GPIO_WritePin(LED->GPIO_Port, LED->GPIO_Pin, LED->state);

	  /*Configure GPIO pin : LED_ONBOARD_Pin */
	  GPIO_InitStruct.Pin = LED->GPIO_Pin;
	  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	  GPIO_InitStruct.Pull = GPIO_NOPULL;
	  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	  HAL_GPIO_Init(LED->GPIO_Port, &GPIO_InitStruct);
}


void LED_Toggle(LED_t *LED)
{
	LED->state ^= 1; // Inverts state
	HAL_GPIO_TogglePin(LED->GPIO_Port, LED->GPIO_Pin);
}


void LED_On(LED_t *LED)
{
	LED->state = 1;
	HAL_GPIO_WritePin(LED->GPIO_Port, LED->GPIO_Pin, 1);
}

void LED_Off(LED_t *LED)
{
	LED->state = 0;
	HAL_GPIO_WritePin(LED->GPIO_Port, LED->GPIO_Pin, 0);
}

_Bool LED_Get_state(LED_t *LED)
{
	return LED->state;
}
