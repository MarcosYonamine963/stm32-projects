#include <stdint.h>
#include "stm32f0xx.h"
#include "uart.h"
#include "convert.h"

uint16_t adc_data = 0;
uint8_t adc_res_string[10];

void echo(uint8_t data);

int main(void)
{

	Uart_Config(USART1, 19200, echo);
	Uart_Enable(USART1);

	// Configurar ADC no pino do 1-Wire para atuar como leitor de sensor analogico

	// PA4: Pino 1-Wire
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;
	// Configura PA4 como input analog
	// Configurar pino como analogico:
	// MODER: 0b11
	// PUPD: 0b00
	GPIOA->MODER |=  GPIO_MODER_MODER4; // Analog Mode
	GPIOA->PUPDR &=  ~(GPIO_PUPDR_PUPDR4); // No pull

	// Habilita ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADCEN;

	// talvez precise habilitar o clock HSI14
//	RCC->CR2 |= RCC_CR2_HSI14ON;
//	while( !(RCC->CR2 & RCC_CR2_HSI14RDY) );

	// ADC: 12bit, 10bit, 8bit, 6bit configurable resolution
	// Modes: single, continuous, scan or discontinuous
	// Result is stored in 16-bit data register
	// ADC input range: VSSA ≤ VIN ≤ VDDA

	// Habilita ADC
	if ((ADC1->ISR & ADC_ISR_ADRDY) != 0)
	{
	 ADC1->ISR |= ADC_ISR_ADRDY;
	}
	ADC1->CR |= ADC_CR_ADEN;
	while ((ADC1->ISR & ADC_ISR_ADRDY) == 0);

	// Seleciona HSI14 como clock do ADC
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;
	RCC->CR2 |= RCC_CR2_HSI14ON;
	while ((RCC->CR2 & RCC_CR2_HSI14RDY) == 0);
	ADC1->CFGR2 &= (~ADC_CFGR2_CKMODE); // seleciona HSI14 como source

	// Seleciona continuous mode e scanning direction
	ADC1->CFGR1 |= ADC_CFGR1_CONT | ADC_CFGR1_SCANDIR;
	// Seleciona Canal 4 para leitura
	ADC1->CHSELR = ADC_CHSELR_CHSEL4;
	// Seleciona sampling mode como 0b111
	ADC1->SMPR |= ADC_SMPR_SMP_0 | ADC_SMPR_SMP_1 | ADC_SMPR_SMP_2;
	// Enable interrupts on EOC, EOSEQ and overrun
	ADC1->IER = ADC_IER_EOCIE | ADC_IER_EOSEQIE | ADC_IER_OVRIE;
	// NVIC: Enable Interrupt on ADC
	NVIC_EnableIRQ(ADC1_IRQn);
	// Set priority for ADC
	NVIC_SetPriority(ADC1_IRQn,0);
	// Inicia conversao com ADSTART=1
	ADC1->CR |= ADC_CR_ADSTART;



	Uart_Write_Text(USART1, "System Init Complete\r\n");

    while(1)
    {
    	// Loop embarcado vazio.
    	// Tudo feito por interrupcao
    }
}

void ADC_IRQHandler(void)
{
//	Uart_Write_Text(USART1, "Interrupt do ADC\r\n");
	if(ADC1->ISR & ADC_ISR_EOC)
	{
		adc_data = (0x0FFF)&(uint16_t)ADC1->DR; // ADC tem 12 bits
		adc_data = ((uint32_t)adc_data * 5000) / 3950;
		Uint16_To_NAscii(adc_data/1000, adc_res_string);
		Uart_Write_Text(USART1, (char *)(adc_res_string+3) );
		Uart_Write_Text(USART1, ".");
		Uint16_To_NAscii( (adc_data%1000)/10, adc_res_string);
		Uart_Write_Text(USART1, (char *)(adc_res_string+3) );
		Uart_Write_Text(USART1, "V\r\n");
	}
}

void echo(uint8_t data)
{
	Uart_Write_Byte(USART1, data);
}
