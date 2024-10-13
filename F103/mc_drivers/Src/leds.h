#ifndef LEDS_H_
#define LEDS_H_

#include "stm32f1xx.h"

void Leds_Config(void);

void Leds_Onboard_Led_ON(void);
void Leds_Onboard_Led_OFF(void);
void Leds_Onboard_Led_Toggle(void);

void Leds_Red_Led_ON(void);
void Leds_Red_Led_OFF(void);
void Leds_Red_Led_Toggle(void);

void Leds_Green_Led_ON(void);
void Leds_Green_Led_OFF(void);
void Leds_Green_Led_Toggle(void);

#endif /* LEDS_H_ */
