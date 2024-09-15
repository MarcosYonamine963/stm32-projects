/*
 * main.h
 *
 *  Created on: Aug 26, 2024
 *      Author: Mc
 */

#ifndef MAIN_H_
#define MAIN_H_

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "stm32f1xx.h"
#include "sys_clock.h"
#include "timer.h"
#include "gpio.h"
#include "exti.h"
#include "button.h"
#include "uart.h"
#include "buzzer.h"
#include "leds.h"


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define DEBUG_ENABLE        // comment this line to disable uart debug transmissions

#define UART_MAIN           USART1
#define UART_DEBUG          USART2

#define BUTTON0_PORT        GPIOB
#define BUTTON0_PIN         0

#define BUTTON1_PORT        GPIOB
#define BUTTON1_PIN         1

#define BUZZER_PORT         GPIOB
#define BUZZER_PIN          14


#endif /* MAIN_H_ */
