/*
 * led_driver.c
 *
 *  Created on: Jul 18, 2024
 *      Author: Mc
 */


#include "led_driver.h"

void led_driver_init(led_typedef_t *led)
{
    // Enable gpio clock
    switch( (uint32_t)led->GPIO_PORT )
    {
        case (uint32_t)GPIOA:
            // Enable APB2 GPIOA clock
            RCC->APB2ENR |= (1<<2);
        break;

        case (uint32_t)GPIOB:
            // Enable APB2 GPIOB clock
            RCC->APB2ENR |= (1<<3);
        break;

        case (uint32_t)GPIOC:
            // Enable APB2 GPIOC clock
            RCC->APB2ENR |= (1<<4);
        break;

        default:
            return;
    }// switch

    // mask for set register bits
    uint32_t out_mode_mask = 0b0001; // default: push-pull

    if(led->output_mode == OUTPUT_OPEN_DRAIN)
    {
        out_mode_mask = 0b0101;
    }

    // config gpio output
    if(led->GPIO_PIN > 7)
    {
        led->GPIO_PORT->CRH &= ~( (0b1111)<<( (led->GPIO_PIN - 8)*4 ) );
        led->GPIO_PORT->CRH |=  ( (out_mode_mask)<<( (led->GPIO_PIN - 8)*4 ) );
    }
    else
    {
        led->GPIO_PORT->CRL &= ~( (0b1111)<<( (led->GPIO_PIN)*4 ) );
        led->GPIO_PORT->CRL |=  ( (out_mode_mask)<<( (led->GPIO_PIN)*4 ) );
    }

    // led start state: OFF
    led_driver_off(led);
    led->state = LED_DRIVER_STATE_OFF;

}// end led_init

void led_driver_toggle(led_typedef_t *led)
{
    led->GPIO_PORT->ODR ^= ( (1)<<(led->GPIO_PIN) );
    led->state ^=1;
}// end led_toggle

void led_driver_on(led_typedef_t *led)
{
    if(led->active_level == LED_ACTIVE_HIGH)
    {
        led->GPIO_PORT->ODR |= ( (1)<<(led->GPIO_PIN) );
    }
    else
    {
        led->GPIO_PORT->ODR &= ~( (1)<<(led->GPIO_PIN) );
    }
    led->state = LED_DRIVER_STATE_ON;
}// end led_on

void led_driver_off(led_typedef_t *led)
{
    if(led->active_level == LED_ACTIVE_HIGH)
    {
        led->GPIO_PORT->ODR &= ~( (1)<<(led->GPIO_PIN) );
    }
    else
    {
        led->GPIO_PORT->ODR |= ( (1)<<(led->GPIO_PIN) );
    }

    led->state = LED_DRIVER_STATE_OFF;
}
