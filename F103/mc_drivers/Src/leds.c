/*
 * leds.c
 *
 *  Created on: Jul 18, 2024
 *      Author: Mc
 */

#include "led_driver.h"

#define LED_ONBOARD_PORT  GPIOC
#define LED_ONBOARD_PIN   13

#define LED_GREEN_PORT    GPIOB
#define LED_GREEN_PIN     6

#define LED_RED_PORT GPIOA
#define LED_RED_PIN  5

led_typedef_t led_onboard = {   LED_ONBOARD_PORT,
                                LED_ONBOARD_PIN,
                                OUTPUT_PUSH_PULL,
                                LED_ACTIVE_LOW,
                                LED_STATE_OFF};

led_typedef_t led_green = {     LED_GREEN_PORT,
                                LED_GREEN_PIN,
                                OUTPUT_PUSH_PULL,
                                LED_ACTIVE_HIGH,
                                LED_STATE_OFF};

led_typedef_t led_red = {       LED_RED_PORT,
                                LED_RED_PIN,
                                OUTPUT_PUSH_PULL,
                                LED_ACTIVE_HIGH,
                                LED_STATE_OFF};

void Leds_Init(void)
{
    led_driver_init(&led_onboard);
    led_driver_init(&led_green);
    led_driver_init(&led_red);
}

void Leds_Toggle_Onboard_Led(void)
{
    led_driver_toggle(&led_onboard);
}

void Leds_Toggle_Green_Led(void)
{
    led_driver_toggle(&led_green);
}

void Leds_Toggle_Red_Led(void)
{
    led_driver_toggle(&led_red);
}
