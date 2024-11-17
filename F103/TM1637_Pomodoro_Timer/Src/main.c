#include <stdint.h>
#include "stm32f1xx.h"
#include "stm32f103drivers/sys_clock.h"
#include "stm32f103drivers/timer.h"
#include "stm32f103drivers/gpio.h"
#include "stm32f103drivers/exti.h"
#include "stm32f103drivers/uart.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

int main(void)
{

    // Configure System Clock (HSI, 64 MHz)
    Sys_Clock_Init();

    // Init Timers
    Timer_Init();

    // Embedded loop
    while(1)
    {

    }// end Embedded loop

}// end main func
