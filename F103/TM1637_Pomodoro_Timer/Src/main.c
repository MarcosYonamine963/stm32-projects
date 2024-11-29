#include <stdint.h>
#include "stm32f1xx.h"
#include "stm32f103drivers/sys_clock.h"
#include "stm32f103drivers/timer.h"
#include "serial.h"
#include "buttons.h"
#include "leds.h"
#include "action.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif


int main(void)
{

    // Configure System Clock (HSI, 64 MHz)
    Sys_Clock_Init();

    // Init Timers
    Timer_Init();

    // Serial Communication
    Serial_Config(19200);

    // Leds
    Leds_Config();

    // Events and Actions
    Action_Config();

    Serial_Send_Text("System Init Complete\r\n");

    // Embedded loop
    while(1)
    {
        Timer_SM();

        Serial_SM();

        Action_SM();

        uint8_t data;
        if(Serial_Read_Data(&data) == SERIAL_OK)
        {
            Serial_Send_Byte(data);
        }


    }// end Embedded loop

}// end main func
