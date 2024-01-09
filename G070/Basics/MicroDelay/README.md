# What is this project?

This is a simple lib for STM32 devices to implement microsecond timer and delays, like the millis on arduino, or HAL_GetTick on STM.

It's a very simple lib (if it IS called a lib...), based on this [website](https://copyprogramming.com/howto/stm32-create-a-microsecond-timer), with some basic changes.

# How to use this lib

1. Create a project using STMCubeIDE, in language C.
2. Copy/paste the files micro_delay.c and micro_delay.h to project tree
3. Include the .h file at main.c
4. Edit the SysTick_Handler function at file stm32g0xx_it.c:

       volatile uint32_t micro_update_micro;
       void SysTick_Handler(void)
       {
           micro_update_micro++;
           HAL_IncTick();
       }

5. At file micro_delay.c, edit the include line for stm32 correct family
6. Use the functions GetMicros() or GetMicrosFromISR(). They work like HAL_GetTick() (or millis() from arduino), but values are in microseconds.
