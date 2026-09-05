#ifndef UART_RX_H
#define UART_RX_H

#include <stdint.h>
#include "flood_types.h"

typedef struct {
	uint16_t level_mm;
	RiskState risk;
	uint16_t ttc_sec;
	uint8_t pump_pwm;
	uint8_t power_stage;
	uint8_t error_code;
	uint8_t communication_ok;
} DisplayData;

void uart0_init(void);
uint8_t uart0_poll_status(DisplayData *data);

#endif