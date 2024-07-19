/*
 * led.h
 *
 *  Created on: Jul 18, 2024
 *      Author: Mc
 */

#ifndef LED_DRIVER_H_
#define LED_DRIVER_H_

#include "stm32f1xx.h"

typedef enum
{
    OUTPUT_PUSH_PULL = 0,
    OUTPUT_OPEN_DRAIN
}led_output_mode_e;

typedef enum
{
    LED_ACTIVE_HIGH = 0,
    LED_ACTIVE_LOW
}led_active_level_e;

typedef enum
{
    LED_STATE_OFF = 0,
    LED_STATE_ON
}led_state_e;


typedef struct
{
    GPIO_TypeDef *GPIO_PORT;            /* GPIOA, GPIOB or GPIOC */
    uint8_t GPIO_PIN;                   /* 0 to 15 */
    led_output_mode_e output_mode;
    led_active_level_e active_level;    /* GPIO level to turn led ON */
    led_state_e state;                  /* Flag (read only) */
}led_typedef_t;

void led_driver_init(led_typedef_t *led);
void led_driver_toggle(led_typedef_t *led);
void led_driver_on(led_typedef_t *led);
void led_driver_off(led_typedef_t *led);
led_state_e led_driver_get_state(led_typedef_t *led);


#endif /* LED_DRIVER_H_ */
