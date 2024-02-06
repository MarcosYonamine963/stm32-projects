/*
 * onewire_slave.h
 *
 *  Created on: Jan 12, 2024
 *      Author: mcmachine
 */

#ifndef INC_ONEWIRE_SLAVE_H_
#define INC_ONEWIRE_SLAVE_H_

#ifdef __cplusplus
extern "C"
{
#endif

#include "inttypes.h"




typedef enum
{
	READ_CMD = 0,
	SEND_ID
}Onewire_State_Machine_t;

typedef struct
{
	uint32_t bus_low_time_read;
	uint32_t __micro_timer_value;

	uint8_t counter;
	uint8_t cmd_read;
	_Bool reset; // if just have reseted (its for bug fix)

	uint8_t onewire_id_to_send[8]; // 8 bytes of ID to send
	uint8_t ROM_ID[8];

	Onewire_State_Machine_t onewire_state;
	uint8_t onewire_sent_counter;

}OneWire_t;


void onewire_init(OneWire_t *OneWire);

void onewire_exti_rising_handler(OneWire_t *OneWire);
void onewire_exti_falling_handler(OneWire_t *OneWire);

_Bool onewire_verify_presence(OneWire_t *OneWire);

void onewire_enable_interrupt(void);
void onewire_disable_interrupt(void);

_Bool onewire_communication(OneWire_t *OneWire);


#ifdef __cplusplus
}
#endif

#endif /* INC_ONEWIRE_SLAVE_H_ */
