/*
 * onewire_slave.c
 *
 *  Created on: Jan 12, 2024
 *      Author: mcmachine
 */


#include "onewire_slave.h"
#include "micro_delay.h"
#include "main.h"
#include "stdio.h"

/* ################################ */

#define ONE_WIRE_TX_LOW		(ONEWIRE_TX_GPIO_Port->ODR |= ONEWIRE_TX_Pin)
#define ONE_WIRE_TX_HIGH	(ONEWIRE_TX_GPIO_Port->ODR &= ~ONEWIRE_TX_Pin)
#define ONE_WIRE_BUS_LEVEL	(ONEWIRE_GPIO_Port->IDR & ONEWIRE_Pin)

/* ################################ */
#define BIT_ONE_TIME 	20
#define BIT_ZERO_TIME	40



/* ################################ */


static void onewire_send_bit_zero(void);
static void onewire_send_bit_one(void);
static void onewire_send_ID(uint8_t ID_Vector[8]); // send 8 bytes
static void onewire_send_ROM_ADDR(uint8_t ROM_Vector[8]);
static void onewire_send_presence(void);
static uint8_t onewire_read_byte(void);
static _Bool onewire_read_bit(void);

/* ################################ */

/* Exported Functions */

void onewire_init(OneWire_t *OneWire)
{
	OneWire->cmd_read = 0;
	OneWire->counter = 0;
	OneWire->onewire_state = READ_CMD;
	OneWire->onewire_sent_counter = 0;

	ONE_WIRE_TX_HIGH;

//	onewire_disable_interrupt();
}

void onewire_enable_interrupt(void)
{
	HAL_NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
	HAL_NVIC_EnableIRQ(EXTI0_1_IRQn);
}

void onewire_disable_interrupt(void)
{
	HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
	HAL_NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
}

_Bool onewire_verify_presence(OneWire_t *OneWire)
{
	onewire_enable_interrupt();
	if(OneWire->reset && (micro_get_val() - OneWire->__micro_timer_value < 560))
	{
//		printf("Presence\r\n");
		onewire_disable_interrupt();
		OneWire->reset = 0;
		return 1;
	}
	return 0;
}

/*
 * Run this inside a while, blocking processor (major priority)
 *
 * if communication done, return 0
 */
_Bool onewire_communication(OneWire_t *OneWire)
{
	typedef enum
	{
		START = 0,
		READ_ROM_CMD,
//		SEND_ROM,// if cmd is 0xF0
		EXIT
	}OneWire_State_e;

	static OneWire_State_e onewire_state = START;

	switch(onewire_state)
	{
		case START:

			if(onewire_verify_presence(OneWire))
			{
//				onewire_state = SEND_PRESENCE;
//				printf("Presence\r\n");

				onewire_send_presence();
				onewire_state = READ_ROM_CMD;

			}
			else return 0;

			break;

		case READ_ROM_CMD:

			uint8_t cmd = 0;
			cmd = onewire_read_byte();

			if(cmd == 0x33)// Read ROM
			{
//				onewire_state = SEND_ID;
				onewire_send_ID(OneWire->onewire_id_to_send);
				onewire_state = START;
			}
			else if (cmd == 0xF0) // Search ROM
			{
//				onewire_state = SEND_ROM;
				onewire_send_ROM_ADDR(OneWire->ROM_ID);
				onewire_state = EXIT;
			}
			else
			{
				onewire_state = EXIT;
				printf("CMD: %X\r\n", cmd);
			}

			break;

//		case SEND_ROM:
//
//
//
//
//			break;


		case EXIT:
			onewire_enable_interrupt();
			onewire_state = START;
			return 0;
			break;


		default:
			onewire_enable_interrupt();
			onewire_state = START;
			return 0;
			break;
	}// end switch(onewire_state)

	return 1; // still in communication

}// end onewire_communication




void onewire_exti_rising_handler(OneWire_t *OneWire)
{
	OneWire->bus_low_time_read = micro_get_val() - OneWire->__micro_timer_value;

	if(OneWire->bus_low_time_read  > 400 && OneWire->bus_low_time_read < 540)
	{
		OneWire->reset = 1;
	}
	else
	{
		OneWire->reset = 0;
	}
}

void onewire_exti_falling_handler(OneWire_t *OneWire)
{

	OneWire->__micro_timer_value = micro_get_val();
}

/* ################################ */

/* Private Functions */

//static void onewire_exti_handler(OneWire_t *OneWire, _Bool rising)
//{
//
//	switch(OneWire->onewire_state)
//	{
//
//		case READ_CMD:
//
//			if(rising &&  OneWire->bus_low_time_read >= 70 && OneWire->bus_low_time_read < 150) // bit 0
//			{
//				OneWire->counter++;
//			}
//			else if(rising && OneWire->bus_low_time_read < 50) // bit 1
//			{
//
//				if(OneWire->counter < 4)
//				{
//
//					OneWire->cmd_read |= (1 << (OneWire->counter + 4));
//					OneWire->counter++;
//
//				}
//				else
//				{
//					OneWire->cmd_read |= (1 << (OneWire->counter - 4));
//					OneWire->counter++;
//				}
//			}
//
//			if(OneWire->reset)
//			{
//				OneWire->counter = 0; // to ignore one interrupt
//				OneWire->reset = 0;
//			}
//
//			if(OneWire->counter == 8)
//			{
//
//				if(OneWire->cmd_read == 0x33)
//				{
//					HAL_NVIC_DisableIRQ(EXTI0_1_IRQn);
////					HAL_NVIC_ClearPendingIRQ(EXTI0_1_IRQn);
//
//					OneWire->onewire_state = SEND_ID;
//				}
//
//				// reset state
//				OneWire->counter = 0;
//				OneWire->cmd_read = 0;
//
//			}
//
//			break;
//
//		case SEND_ID:
//
//
//			break;
//
//		default:
//			break;
//	}
//
//}// end onewire_exti_handler




static void onewire_send_presence(void)
{
	ONE_WIRE_TX_LOW;
	micro_delay(100);
	ONE_WIRE_TX_HIGH;
}




static uint8_t onewire_read_byte(void)
{
	uint8_t value = 0;

	for(int i = 0; i < 4;)// second nibble
	{
		if(!ONE_WIRE_BUS_LEVEL)
		{
			micro_delay(50);
			if(ONE_WIRE_BUS_LEVEL)
			{
				value |= (1 << i);
			}
//			micro_delay(50);
			i++;
			while(!ONE_WIRE_BUS_LEVEL);
		}
	}

	for(int i = 4; i < 8;)// first nibble
	{
		if(!ONE_WIRE_BUS_LEVEL)
		{
			micro_delay(50);
			if(ONE_WIRE_BUS_LEVEL)
			{
				value |= (1 << i);
			}
//			micro_delay(50);
			i++;
			while(!ONE_WIRE_BUS_LEVEL);
		}
	}

	return value;

}



static void onewire_send_ID(uint8_t ID_Vector[8])
{
	for(int byte_idx = 0; byte_idx < 8;)
	{

		for(int bit_idx = 0; bit_idx < 8;)
		{

			if( !( (ID_Vector[byte_idx]) & (1 << bit_idx) ) )
			{
				onewire_send_bit_zero();
			}
			else
			{
				onewire_send_bit_one();
			}
			bit_idx++;

		}// end for

		byte_idx++;

	}// end for byte_idx

}// end onewire_send_ID

/* ################################ */

static void onewire_send_ROM_ADDR(uint8_t ROM_Vector[8])
{
	_Bool temp;

	for(int byte_idx = 0; byte_idx < 8; byte_idx++)
	{

		for(int bit_idx = 0; bit_idx < 8; bit_idx++)
		{

			if( !( (ROM_Vector[byte_idx]) & (1 << bit_idx) ) )
			{
				onewire_send_bit_zero();
				onewire_send_bit_one();
				temp = onewire_read_bit();
				if(temp)
				{
					return;
				}
			}
			else
			{
				onewire_send_bit_one();
				onewire_send_bit_zero();
				temp = onewire_read_bit();
				if(!temp)
				{
					return;
				}
			}
		}// end for bit_idx first nibble
	}// end for byte_idx
}// end onewire_send_ROM_ADDR





// LOW LEVEL FUNCTIONS (BIT LEVEL)


static void onewire_send_bit_zero(void)
{
	uint32_t timer = micro_get_val();

	do
	{
		if(!ONE_WIRE_BUS_LEVEL)
		{
			ONE_WIRE_TX_LOW;
			micro_delay(BIT_ZERO_TIME);
			ONE_WIRE_TX_HIGH;
//			micro_delay(10);
			while(!ONE_WIRE_BUS_LEVEL);
			return;
		}


	}while(micro_get_val() - timer < 1000);

	return;

}

static void onewire_send_bit_one(void)
{

	uint32_t timer = micro_get_val();

	do
	{
		if(!ONE_WIRE_BUS_LEVEL)
		{
//			micro_delay(BIT_ONE_TIME);
			while(!ONE_WIRE_BUS_LEVEL);
			return;
		}


	}while(micro_get_val() - timer < 1000);

	return;

}


static _Bool onewire_read_bit(void)
{
	uint32_t timer = micro_get_val();
	_Bool value = 0;

	do
	{
		if(!ONE_WIRE_BUS_LEVEL)
		{
			micro_delay(30);
			value = ONE_WIRE_BUS_LEVEL;
			while(!ONE_WIRE_BUS_LEVEL);
			return value;
		}
	}while(micro_get_val() - timer < 1000);

	return value;


}
