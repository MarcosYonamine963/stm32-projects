#include <stdint.h>
#include "stm32f1xx.h"
#include "stm32f103drivers/sys_clock.h"
#include "stm32f103drivers/timer.h"
#include "serial.h"
#include "buttons.h"
#include "leds.h"
#include "action.h"
#include "stm32f103drivers/i2c.h"

#if !defined(__SOFT_FP__) && defined(__ARM_FP)
  #warning "FPU is not initialized, but the project is compiling for an FPU. Please initialize the FPU before use."
#endif

void teste_i2c(void);

int main(void)
{

    // Configure System Clock (HSI, 64 MHz)
    Sys_Clock_Init();

    // Init Timers
    Timer_Init();

    // Serial Communication
    Serial_Config(19200);
    Serial_Send_Text("Uart Serial configured\r\n");

    // Leds
    Leds_Config();
    Serial_Send_Text("Leds configured\r\n");

    // Events and Actions
    Action_Config();
    Serial_Send_Text("Events and Actions configured\r\n");

    // I2C config
    I2C1_Config(I2C1_PIN_REMAP_NO);
    Serial_Send_Text("I2C1 configured\r\n");

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

        teste_i2c();


    }// end Embedded loop

}// end main func


void teste_i2c(void)
{
    static uint8_t onetime = 0;

    if(onetime)
        return;

    onetime = 1;

    uint8_t i2c_addrs[128];
    uint8_t i2c_addr_quant;

    i2c_addr_quant = I2C1_Scan_Bus(i2c_addrs);
    if(i2c_addr_quant)
    {
        for(uint8_t i = 0; i < i2c_addr_quant; i++)
        {
            Serial_Send_Text("I2C addr found: ");
            Serial_Send_Byte(i2c_addrs[i]);
            Serial_Send_Text("\r\n");
        }
    }

}// end teste_i2c
