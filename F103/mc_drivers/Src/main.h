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
#include "encoder.h"
#include "tm1637_led_display.h"
#include "my_clock.h"


#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

#define DEBUG_ENABLE        1   // uart debug transmissions. 1: enable. 0: disable

#define UART_MAIN           USART1
#define UART_DEBUG          USART3

#define BUTTON0_PORT        GPIOB
#define BUTTON0_PIN         14

#define BUTTON1_PORT        GPIOB
#define BUTTON1_PIN         15

#define BUZZER_PORT         GPIOA
#define BUZZER_PIN          8

#define ENCODER_CLK_PORT    GPIOB
#define ENCODER_CLK_PIN     3
#define ENCODER_DT_PORT     GPIOB
#define ENCODER_DT_PIN      4
#define ENCODER_SW_PORT     GPIOB
#define ENCODER_SW_PIN      5

#define TM1637_CLK_PORT     GPIOB
#define TM1637_CLK_PIN      6
#define TM1637_DIO_PORT     GPIOB
#define TM1637_DIO_PIN      7


#endif /* MAIN_H_ */
