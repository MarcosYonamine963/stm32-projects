/*
 * exti.h
 *
 *  Created on: Aug 20, 2024
 *      Author: Mc
 */

#ifndef EXTI_H_
#define EXTI_H_

#include "stm32f1xx.h"

typedef void (*Exti_CallbackFunc_t )(void);

typedef enum
{
    EXTI_LINE_0 = 0,
    EXTI_LINE_1,
    EXTI_LINE_2,
    EXTI_LINE_3,
    EXTI_LINE_4,
    EXTI_LINE_5,
    EXTI_LINE_6,
    EXTI_LINE_7,
    EXTI_LINE_8,
    EXTI_LINE_9,
    EXTI_LINE_10,
    EXTI_LINE_11,
    EXTI_LINE_12,
    EXTI_LINE_13,
    EXTI_LINE_14,
    EXTI_LINE_15,
}exti_line_e;

typedef enum
{
    EXTI_RISING_IT_TRIGGER = 0,
    EXTI_FALLING_IT_TRIGGER,
    EXTI_RISING_FALLING_IT_TRIGGER
}exti_trigger_mode_e;

typedef enum
{
    EXTI_STATUS_OK = 0,
    EXTI_STATUS_ERR_LINE_USED
}exti_status_e;

exti_status_e Exti_config_source(exti_line_e line, GPIO_TypeDef *GPIO, exti_trigger_mode_e mode);
void Exti_config_callback_line(uint8_t line, Exti_CallbackFunc_t callback);
void Exti_Disable_All_Lines();

#endif /* EXTI_H_ */
