#include <stdint.h>
#include "stm32f1xx.h"



#define LED_ON	(  GPIOC->ODR &= ~(1<<13)  ) // LED ON when Pin is LOW
#define LED_OFF	(  GPIOC->ODR |=  (1<<13)  ) // LED OFF when Pin is HIGH

#define READ_BUTTON (  GPIOA->IDR & (1<<0)  )



void GPIO_Init(void);

int main(void)
{

	GPIO_Init();


	while(1)
	{
		if(READ_BUTTON) LED_ON;
		else LED_OFF;

	}

	return 0;
}


void GPIO_Init(void)
{
	/* LED: PC.13 */
	RCC->APB2ENR |= (1<<4); // Enable clock for GPIOC
	GPIOC->CRH |= 	(0b11 << 20); // PC.13 Output mode
	GPIOC->CRH &=  ~(0b11 << 22); // PC.13 Push Pull

	/* Button: PA.0 */
	RCC->APB2ENR |= (1<<2); // Enable clock for GPIOA
	GPIOA->CRL &= ~(1111<<0); // Input mode (resetting all 4 bits of CNF0 + MODE0)
	GPIOA->CRL |= (1<<3); // Set bit 1 of CNF (input pull up / pull down);
}
