#ifndef LEDS_H_
#define LEDS_H_

void Leds_Config(void);

uint8_t Leds_Onboard_Led_ON();
uint8_t Leds_Onboard_Led_OFF();
uint8_t Leds_Onboard_Led_Toggle();

uint8_t Leds_Red_Led_ON();
uint8_t Leds_Red_Led_OFF();
uint8_t Leds_Red_Led_Toggle();

uint8_t Leds_Green_Led_ON();
uint8_t Leds_Green_Led_OFF();
uint8_t Leds_Green_Led_Toggle();

#endif /* LEDS_H_ */
