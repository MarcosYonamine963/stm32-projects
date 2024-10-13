#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f1xx.h"

typedef void (*Encoder_CallbackFunc_t )(void);

void Encoder_Config_clk(GPIO_TypeDef *GPIOx, uint8_t PIN);
void Encoder_Config_dt(GPIO_TypeDef *GPIOx, uint8_t PIN);
void Encoder_Config_sw(GPIO_TypeDef *GPIOx, uint8_t PIN);
void Encoder_Config_callbacks(  Encoder_CallbackFunc_t clk_wise_callback,           \
                                Encoder_CallbackFunc_t counter_clk_wise_callback,   \
                                Encoder_CallbackFunc_t sw_callback);

void Encoder_Init();

#endif /* ENCODER_H_ */
