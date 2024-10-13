#include "leds.h"
#include "gpio.h"
#include "params.h"
#include "uart.h"
#include "stdio.h"
#include "string.h"

#define LED_ONBOARD_PORT    GPIOC
#define LED_ONBOARD_PIN     13

#define LED_RED_PORT        GPIOB
#define LED_RED_PIN         12

#define LED_GREEN_PORT      GPIOB
#define LED_GREEN_PIN       13

static char msg[128]; // Debug message

static void Leds_Debug_Send(uint8_t *msg, uint8_t len, _Bool send_cr_lf)
{
    if( !( (params.debug_cfg) & (1<<2) ) )
    {
        return;
    }

    debug_send_msg(msg, len);
    if(send_cr_lf)
    {
        debug_send_msg((uint8_t *)"\r\n", strlen("\r\n"));
    }
}


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
    sprintf((char *)msg, "Led Onboard ON");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 0);
}

void Leds_Onboard_Led_OFF(void)
{
    sprintf((char *)msg, "Led Onboard OFF");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, 1);
}

void Leds_Onboard_Led_Toggle(void)
{
    sprintf((char *)msg, "Led Onboard Toggle");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_ONBOARD_PORT, LED_ONBOARD_PIN, \
            Gpio_Digital_Read(LED_ONBOARD_PORT, LED_ONBOARD_PIN) ^ 1 );
}

void Leds_Red_Led_ON(void)
{
    sprintf((char *)msg, "Led Vermelho ON");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);


    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, 1);
}

void Leds_Red_Led_OFF(void)
{
    sprintf((char *)msg, "Led Vermelho OFF");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, 0);
}

void Leds_Red_Led_Toggle(void)
{
    sprintf((char *)msg, "Led Vermelho Toggle");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_RED_PORT, LED_RED_PIN, \
            Gpio_Digital_Read(LED_RED_PORT, LED_RED_PIN) ^ 1 );
}

void Leds_Green_Led_ON(void)
{
    sprintf((char *)msg, "Led Verde ON");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, 1);
}

void Leds_Green_Led_OFF(void)
{
    sprintf((char *)msg, "Led Verde OFF");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, 0);
}

void Leds_Green_Led_Toggle(void)
{
    sprintf((char *)msg, "Led Verde Toggle");
    Leds_Debug_Send((uint8_t *)msg, strlen((char *)msg), 1);

    Gpio_Digital_Write(LED_GREEN_PORT, LED_GREEN_PIN, \
            Gpio_Digital_Read(LED_GREEN_PORT, LED_GREEN_PIN) ^ 1 );
}

