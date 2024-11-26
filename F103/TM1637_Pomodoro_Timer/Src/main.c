#include <stdint.h>
#include "stm32f1xx.h"
#include "stm32f103drivers/sys_clock.h"
#include "stm32f103drivers/timer.h"
#include "serial.h"
#include "buttons.h"
#include "leds.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

static void dumb_func(void)
{
    Serial_Send_Text("Olair\r\n");
}

int main(void)
{

    // Configure System Clock (HSI, 64 MHz)
    Sys_Clock_Init();

    // Init Timers
    Timer_Init();

    // Serial Communication
    Serial_Config(19200);

    // Buttons
    Button_Config(BUTTON0, dumb_func, dumb_func, dumb_func, dumb_func);
    Button_Config(BUTTON1, dumb_func, dumb_func, dumb_func, dumb_func);

    // Leds
    Leds_Config();

    Timer_Set(TIMER_GENERIC, TIME_500MS, Leds_Red_Led_Toggle, TIMER_MODE_ALWAYS);

    Serial_Send_Text("System Init Complete\r\n");

    // Embedded loop
    while(1)
    {
        Timer_SM();

        Serial_SM();

        uint8_t data;
        if(Serial_Read_Data(&data) == SERIAL_OK)
        {
            Serial_Send_Byte(data);
        }


    }// end Embedded loop

}// end main func
