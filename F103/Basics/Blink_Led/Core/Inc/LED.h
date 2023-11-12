/*
 * LED.h
 *
 *  Created on: Nov 12, 2023
 *      Author: MarcosYonamine963
 *
 *  Rev: 0
 */

#ifndef INC_LED_H_
#define INC_LED_H_

#include "stm32f1xx_hal.h"

typedef struct
{
	uint32_t GPIO_Port;
	uint32_t GPIO_Pin;
	_Bool state;
}LED_t;

void LED_Init(LED_t *LED);
void LED_Toggle(LED_t *LED);
void LED_On(LED_t *LED);
void LED_Off(LED_t *LED);
_Bool LED_Get_state(LED_t *LED);


#endif /* INC_LED_H_ */
