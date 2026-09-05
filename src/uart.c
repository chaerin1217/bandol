#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "uart.h"

#define UBRR1_VALUE ((F_CPU / (16UL * UART_BAUD)) - 1UL)

static void uart1_put_u16(uint16_t value)
{
	char buffer[6];
	uint8_t index = 0;

	if (value == 0) {
		uart1_putc('0');
		return;
	}

	while (value > 0) {
		buffer[index++] = (char)('0' + (value % 10));
		value /= 10;
	}

	while (index > 0) {
		uart1_putc(buffer[--index]);
	}
}

void uart1_init(void)
{
	UBRR1H = (uint8_t)(UBRR1_VALUE >> 8);
	UBRR1L = (uint8_t)UBRR1_VALUE;

	UCSR1B = (1 << RXEN1) | (1 << TXEN1);
	UCSR1C = (1 << UCSZ11) | (1 << UCSZ10);
}

void uart1_putc(char data)
{
	while (!(UCSR1A & (1 << UDRE1)));
	UDR1 = data;
}

void uart1_puts(const char *str)
{
	while (*str != '\0') {
		uart1_putc(*str++);
	}
}

void uart1_send_status(uint16_t level_mm,
RiskState state,
uint16_t ttc_sec,
const ControlOutput *out,
uint8_t error_code)
{
	uart1_puts("<L=");
	uart1_put_u16(level_mm);

	uart1_puts(",R=");
	uart1_put_u16((uint16_t)state);

	uart1_puts(",T=");
	uart1_put_u16(ttc_sec);

	uart1_puts(",M=");
	uart1_put_u16(out->pump_pwm);

	uart1_puts(",P=");
	uart1_put_u16(out->power_stage);

	uart1_puts(",E=");
	uart1_put_u16(error_code);

	uart1_puts(">\r\n");
}