/*
 * LED.h
 *
 * ATTENTION: To use this lib, all tha bare-metal program
 * must be non-blocking (DON'T USE DELAY FUNCTIONS, OR LONG LOOPS)
 *
 *  Created on: Nov 13, 2023
 *      Author: mcmachine
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32g0xx_hal.h"

typedef struct
{
	GPIO_TypeDef* GPIO_Port;
	uint16_t GPIO_Pin;
	_Bool state;
	uint16_t blink_freq;
	uint32_t __last_tick;
}LED_t;

void LED_Init(LED_t *LED);
void LED_Toggle(LED_t *LED);
void LED_On(LED_t *LED);
void LED_Off(LED_t *LED);
_Bool LED_Get_state(LED_t *LED);
void LED_Set_Blink_Val(LED_t *LED, uint16_t value);

void LED_loop_handle(LED_t *LED);



#endif /* INC_LED_H_ */
