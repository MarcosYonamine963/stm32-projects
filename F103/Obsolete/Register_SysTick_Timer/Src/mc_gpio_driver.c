/*
 * mc_gpio_driver.c
 *
 *  Created on: Jan 12, 2024
 *      Author: mclovin
 */


#include "mc_gpio_driver.h"

void GPIO_Init(unsigned short PORT, unsigned short PIN, unsigned short Direction, unsigned short Option)
{
	volatile unsigned long *CR;
	unsigned short tPIN = PIN;
	unsigned short offset = 0x00;


	if(PIN > 7)
	{
		tPIN -= 8;
		offset = 0x01;
	}

	if(PORT == PA)
	{
		RCC_APB2ENR |= (1<<2); // Enable clock for GPIOA
		CR = (volatile unsigned long *) (&GPIO_A + offset);
	}
	else if(PORT == PB)
	{
		RCC_APB2ENR |= (1<<3); // Enable clock for GPIOA
		CR = (volatile unsigned long *) (&GPIO_B + offset);
	}
	else if(PORT == PC)
	{
		RCC_APB2ENR |= (1<<4); // Enable clock for GPIOA
		CR = (volatile unsigned long *) (&GPIO_C + offset);
	}

	*CR &= ~(0xF <<(tPIN)*4); // reset the target pin
	*CR |= ( (Direction<<(tPIN*4)) | (Option<<(tPIN*4+2)) );// Set up the direction and the option of the PIN


}

int GPIO_Read(unsigned short PORT, unsigned short PIN)
{
	volatile unsigned long *IDR;
	unsigned long offset = 0x02;

	if(PORT == PA)
	{
		IDR = (volatile unsigned long *) (&GPIO_A + offset);
	}
	else if(PORT == PB)
	{
		IDR = (volatile unsigned long *) (&GPIO_B + offset);
	}
	else if(PORT == PC)
	{
		IDR = (volatile unsigned long *) (&GPIO_C + offset);
	}

	return ((*IDR & (1 << PIN))>>PIN);

}


void GPIO_Write(unsigned short PORT, unsigned short PIN, unsigned short state)
{
	volatile unsigned long *ODR;
	unsigned long offset = 0x03;

	if(PORT == PA)
	{
		ODR = (volatile unsigned long *) (&GPIO_A + offset);
	}
	else if(PORT == PB)
	{
		ODR = (volatile unsigned long *) (&GPIO_B + offset);
	}
	else if(PORT == PC)
	{
		ODR = (volatile unsigned long *) (&GPIO_C + offset);
	}

	state ? (*ODR |= (1<<PIN)) : ( *ODR &= ~(1<<PIN) );
}


void GPIO_Toggle(unsigned short PORT, unsigned short PIN)
{
	if(GPIO_Read(PORT, PIN)) GPIO_Write(PORT, PIN, 0);
	else GPIO_Write(PORT, PIN, 1);
}





