/*
 * LED.c
 *
 *  Created on: Nov 13, 2023
 *      Author: mcmachine
 */

#include "LED.h"

/*
 * @brief	Init clock and set GPIO as output
 * 			Set initial LED state
 *
 * @param	LED [input]: pointer to LED_t type
 *
 * @retval	None
 */
void LED_Init(LED_t *LED)
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};

	  /* GPIO Ports Clock Enable */
	  switch((uint32_t)(LED->GPIO_Port))
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

/*
 * @brief	Toggle LED state by inverting GPIO Pin state
 *
 * @param	LED [input]: pointer to LED_t type
 *
 * @retval	None
 */
void LED_Toggle(LED_t *LED)
{
	LED->state ^= 1; // Inverts state
	HAL_GPIO_TogglePin((LED->GPIO_Port), LED->GPIO_Pin);
}

/*
 * @brief	Set LED ON by writing GPIO Pin to HIGH
 *
 * @param	LED [input]: pointer to LED_t type
 *
 * @retval	None
 */
void LED_On(LED_t *LED)
{
	LED->state = 1;
	HAL_GPIO_WritePin(LED->GPIO_Port, LED->GPIO_Pin, 1);
}


/*
 * @brief	Set LED OFF by writing GPIO Pin to LOW
 *
 * @param	LED [input]: pointer to LED_t type
 *
 * @retval	None
 */

void LED_Off(LED_t *LED)
{
	LED->state = 0;
	HAL_GPIO_WritePin(LED->GPIO_Port, LED->GPIO_Pin, 0);
}

/*
 * @brief	Returns the state of Led (1 or 0)
 *
 * @param 	LED [input]: pointer to LED_t type
 *
 * @retval	0 to LED_Off, 1 to LED_On
 *
 */
_Bool LED_Get_state(LED_t *LED)
{
	return LED->state;
}

/*
 * To work properly, must run the LED_loop_handle function at embedded loop.
 *
 * @brief	Set value for time comparing (blink)
 *
 * @param 	LED [input]: pointer to LED_t type
 * @param 	value [input]: integer value to set blink freq
 *
 * @retval	None
 */
void LED_Set_Blink_Val(LED_t *LED, uint16_t value)
{
	LED->blink_freq = value;
}


/*
 * Run this func at embedded loop
 *
 * @brief 	Toggle the LED pin according to configured value LED->blink_freq
 * 			User can manually configure at struct, or use the function LED_Set_Blink_Val
 *
 * @param 	LED [input]: pointer to LED_t type
 *
 * @retval 	None
 */
void LED_loop_handle(LED_t *LED)
{

	if((HAL_GetTick() - LED->__last_tick) > LED->blink_freq)
	{
		LED_Toggle(LED);
		LED->__last_tick = HAL_GetTick();
	}

}
