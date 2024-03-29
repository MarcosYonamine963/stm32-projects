/**
 ******************************************************************************
 * @file           : main.c
 * @author         : Auto-generated by STM32CubeIDE
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

#include <stdint.h>
#include "stm32f1xx.h"
#include "mc_gpio_driver.h"

#define LED_GPIO_Port PC
#define LED_GPIO_Pin  13


#define LED_STATE_ON 	0
#define LED_STATE_OFF	1
#define LED_SET_STATE(LED_STATE) GPIO_Write(LED_GPIO_Port,LED_GPIO_Pin, LED_STATE)
#define LED_GET_STATE GPIO_Read(LED_GPIO_Port,LED_GPIO_Pin)

void systick_init(void);
void delay_millis(unsigned long t);



int main(void)
{
	systick_init();

	GPIO_Init(LED_GPIO_Port, LED_GPIO_Pin, OUT50, OUT_GP_PP);

	while(1)
	{
		LED_SET_STATE(LED_STATE_ON);
		delay_millis(1000);
		LED_SET_STATE(LED_STATE_OFF);
		delay_millis(1000);

	}
}


void systick_init(void)
{
	SysTick->CTRL = 0;
	SysTick->LOAD = 0x00FFFFFF;
	SysTick->VAL = 0;
	SysTick->CTRL = 0b101; // set Clock Source AHB and Enable Counter.
	// The counter loads the value from LOAD and then counts down. On reaching 0, sets COUNTFLAG to 1. COUNTFLAG access: SysTick->CTRL & (1<<16)
}

/* Delay one millisecond */
void __delay_one_ms()
{
	SysTick->LOAD = 8000-1; // for 8 MHz sys clock. LOAD: init val. start decreasing
	SysTick->VAL = 0; // Load decrease until VAL
	while( (SysTick->CTRL & (1<<16) ) == 0); // Count flag: counter start counting down from LOAD. When reaches 0, flag is set to one
}


void delay_millis(unsigned long t)
{
	for(;t>0;t--)
	{
		__delay_one_ms();
	}
}
