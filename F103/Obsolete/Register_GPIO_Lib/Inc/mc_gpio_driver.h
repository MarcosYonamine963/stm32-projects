/*
 * mc_gpio_driver.h
 *
 *  Created on: Jan 12, 2024
 *      Author: mclovin
 */

#ifndef MC_GPIO_DRIVER_H_
#define MC_GPIO_DRIVER_H_

#ifdef __cplusplus
extern "C"
{
#endif

#define RCC_APB2ENR ( *( (volatile unsigned long *)(0x40021000 + 0x18) ) )

#define GPIO_A ( *( (volatile unsigned long *)(0x40010800) ) )
#define GPIO_B ( *( (volatile unsigned long *)(0x40010C00) ) )
#define GPIO_C ( *( (volatile unsigned long *)(0x40011000) ) )

// Ports
#define PA 1
#define PB 2
#define PC 3

// Directions
#define IN 0
#define OUT10 1
#define OUT2 2
#define OUT50 3

// Options for inputs
#define IN_ANALOG 0
#define IN_FLOATING 1
#define IN_PP 2

// Options for outputs
#define OUT_GP_PP 0
#define OUT_GP_OD 1
#define OUT_AF_PP 2
#define OUT_AF_OD 3


void GPIO_Init	(unsigned short PORT, unsigned short PIN, unsigned short Direction, unsigned short Option);
int  GPIO_Read	(unsigned short PORT, unsigned short PIN);
void GPIO_Write	(unsigned short PORT, unsigned short PIN, unsigned short state);
void GPIO_Toggle(unsigned short PORT, unsigned short PIN);


#ifdef __cplusplus
}
#endif

#endif /* MC_GPIO_DRIVER_H_ */
