#include "stm32f1xx.h"
#include "leds.h"
#include "stm32f103drivers/gpio.h"
#include "stm32f103drivers/uart.h"

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
    Leds_Onboard_Led_OFF();
    // Turn Red Led OFF
    Leds_Red_Led_OFF();
    // Turn Green Led OFF
    Leds_Green_Led_OFF();

}



uint8_t Leds_Onboard_Led_ON()
{
    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 0);

    return 0;
}

uint8_t Leds_Onboard_Led_OFF()
{
    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 1);

    return 0;
}

uint8_t Leds_Onboard_Led_Toggle()
{
    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, \
            Gpio_Digital_Read(LED_ONBOARD_PORT, LED_ONBOARD_PIN) ^ 1 );

    return 0;
}

uint8_t Leds_Red_Led_ON()
{
    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, 1);

    return 0;
}

uint8_t Leds_Red_Led_OFF()
{
    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, 0);

    return 0;
}

uint8_t Leds_Red_Led_Toggle()
{
    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, \
            Gpio_Digital_Read(LED_RED_PORT, LED_RED_PIN) ^ 1 );

    return 0;
}

uint8_t Leds_Green_Led_ON()
{
    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, 1);

    return 0;
}

uint8_t Leds_Green_Led_OFF()
{
    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, 0);

    return 0;
}

uint8_t Leds_Green_Led_Toggle()
{
    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, \
            Gpio_Digital_Read(LED_GREEN_PORT, LED_GREEN_PIN) ^ 1 );

    return 0;
}

