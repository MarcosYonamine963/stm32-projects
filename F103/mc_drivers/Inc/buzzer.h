/*
 * buzzer.h
 *
 *  Created on: Sep 14, 2024
 *      Author: Mc
 */

#ifndef BUZZER_H_
#define BUZZER_H_

#include "stm32f1xx.h"



void Buzzer_Config(GPIO_TypeDef *GPIOx, uint8_t Pin);

void Buzzer_short_beep(uint8_t beeps);
void Buzzer_long_beep(uint8_t beeps);

#endif /* BUZZER_H_ */