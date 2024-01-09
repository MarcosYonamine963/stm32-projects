/*
 * micro_delay.h
 */

/*
 * How to use this lib:
 *
 * Create a project using STM32CubeIDE
 *
 * Copy/paste the files micro_delay.c and micro_delay.h to project tree
 *
 * Include this .h file at main.c
 *
 * Edit the SysTick_Handler function at file stm32g0xx_it.c:

	volatile uint32_t micro_update_micro;

	void SysTick_Handler(void)
	{
		micro_update_micro++;
		HAL_IncTick();
	}

 * At file micro_delay.c, edit the include line for stm32 correct family
 * Use the functions GetMicros() or GetMicrosFromISR().
 * They work like HAL_GetTick() (or millis() from arduino), but values are in microseconds
 * */

#ifndef INC_MICRO_DELAY_H_
#define INC_MICRO_DELAY_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include <inttypes.h>

extern volatile uint32_t micro_update_millis; // Declared on stmg0xx_it.c

uint32_t micro_get_val();
uint32_t micro_get_val_from_ISR();


#ifdef __cplusplus
}
#endif

#endif /* INC_MICRO_DELAY_H_ */
