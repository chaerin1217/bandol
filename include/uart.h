#ifndef UART_H
#define UART_H

#include "flood_types.h"

#define UART_BAUD 9600UL

void uart1_init(void);
void uart1_putc(char data);
void uart1_puts(const char *str);

void uart1_send_status(uint16_t level_mm,
RiskState state,
uint16_t ttc_sec,
const ControlOutput *out,
uint8_t error_code);

#endif