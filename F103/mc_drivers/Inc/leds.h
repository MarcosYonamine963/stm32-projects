/*
 * leds.h
 *
 *  Created on: Jul 18, 2024
 *      Author: Mc
 */

#ifndef LEDS_H_
#define LEDS_H_

void Leds_Init(void);

void Leds_Toggle_Onboard_Led(void);
void Leds_Toggle_Green_Led(void);
void Leds_Toggle_Red_Led(void);

void Leds_Onboard_Led_ON(void);
void Leds_Green_Led_ON(void);
void Leds_Red_Led_ON(void);

void Leds_Onboard_Led_OFF(void);
void Leds_Green_Led_OFF(void);
void Leds_Red_Led_OFF(void);

#endif /* LEDS_H_ */
