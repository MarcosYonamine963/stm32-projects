#ifndef GPIO_H_
#define GPIO_H_

#include "stm32f1xx.h"

typedef enum
{
    OUTPUT_PUSH_PULL = 0,
    OUTPUT_OPEN_DRAIN,
    AF_PUSH_PULL,
    AF_OPEN_DRAIN,
    INPUT_ANALOG,
    INPUT_FLOATING,
    INPUT_PULL_UP,
    INPUT_PULL_DOWN
}gpio_mode_e;

void Gpio_Config(GPIO_TypeDef *GPIO, uint8_t Pin, gpio_mode_e mode);

void Gpio_Digital_Write(GPIO_TypeDef *GPIO, uint8_t Pin, _Bool state);
void Gpio_Digital_Toggle(GPIO_TypeDef *GPIO, uint8_t Pin);
_Bool Gpio_Digital_Read(GPIO_TypeDef *GPIO, uint8_t Pin);

void Gpio_Change_Pin_Mode(GPIO_TypeDef *GPIO, uint8_t Pin, gpio_mode_e mode);

#endif /* GPIO_H_ */
