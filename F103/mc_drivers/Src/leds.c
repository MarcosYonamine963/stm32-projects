/*
 * leds.c
 *
 *  Created on: Sep 14, 2024
 *      Author: Mc
 */


#include "leds.h"
#include "gpio.h"

#define LED_ONBOARD_PORT    GPIOC
#define LED_ONBOARD_PIN     13

#define LED_RED_PORT        GPIOB
#define LED_RED_PIN         12

#define LED_GREEN_PORT      GPIOB
#define LED_GREEN_PIN       13


void Leds_Config(void)
{
    // Config LEDs
    Gpio_Config(LED_ONBOARD_PORT, LED_ONBOARD_PIN, OUTPUT_OPEN_DRAIN);
    Gpio_Config(LED_RED_PORT, LED_RED_PIN, OUTPUT_PUSH_PULL);
    Gpio_Config(LED_GREEN_PORT, LED_GREEN_PIN, OUTPUT_PUSH_PULL);

    // Turn Onboard Led OFF
    //Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 1);
    Leds_Onboard_Led_OFF();
    // Turn Red Led OFF
    Leds_Red_Led_OFF();
    // Turn Green Led OFF
    Leds_Green_Led_OFF();

}



void Leds_Onboard_Led_ON(void)
{
    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 0);
}

void Leds_Onboard_Led_OFF(void)
{
    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 1);
}

void Leds_Onboard_Led_Toggle(void)
{
    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, \
            Gpio_Digital_Read(LED_ONBOARD_PORT, LED_ONBOARD_PIN) ^ 1 );
}

void Leds_Red_Led_ON(void)
{
    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, 1);
}

void Leds_Red_Led_OFF(void)
{
    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, 0);
}

void Leds_Red_Led_Toggle(void)
{
    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, \
            Gpio_Digital_Read(LED_RED_PORT, LED_RED_PIN) ^ 1 );
}

void Leds_Green_Led_ON(void)
{
    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, 1);
}

void Leds_Green_Led_OFF(void)
{
    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, 0);
}

void Leds_Green_Led_Toggle(void)
{
    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, \
            Gpio_Digital_Read(LED_GREEN_PORT, LED_GREEN_PIN) ^ 1 );
}
