#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include "uart_rx.h"

#define UART_BAUD 9600UL
#define UBRR0_VALUE ((F_CPU / (16UL * UART_BAUD)) - 1UL)

#define RX_BUFFER_SIZE 64

static uint8_t read_field(const char *packet,
char field_name,
uint16_t *value)
{
	const char *p = packet;
	uint16_t result = 0;
	uint8_t digit_count = 0;

	while (*p != '\0') {
		if (*p == field_name && *(p + 1) == '=') {
			p += 2;

			while (*p >= '0' && *p <= '9') {
				result = (uint16_t)(result * 10U + (*p - '0'));
				p++;
				digit_count = 1;
			}

			*value = result;
			return digit_count;
		}

		p++;
	}

	return 0;
}

static uint8_t parse_status_packet(const char *packet,
DisplayData *data)
{
	uint16_t level;
	uint16_t risk;
	uint16_t ttc;
	uint16_t pump;
	uint16_t power;
	uint16_t error;

	if (!read_field(packet, 'L', &level)) return 0;
	if (!read_field(packet, 'R', &risk)) return 0;
	if (!read_field(packet, 'T', &ttc)) return 0;
	if (!read_field(packet, 'M', &pump)) return 0;
	if (!read_field(packet, 'P', &power)) return 0;
	if (!read_field(packet, 'E', &error)) return 0;

	if (risk > RISK_ERROR) return 0;
	if (pump > 100) return 0;
	if (power > 3) return 0;

	data->level_mm = level;
	data->risk = (RiskState)risk;
	data->ttc_sec = ttc;
	data->pump_pwm = (uint8_t)pump;
	data->power_stage = (uint8_t)power;
	data->error_code = (uint8_t)error;
	data->communication_ok = 1;

	return 1;
}

void uart0_init(void)
{
	UBRR0H = (uint8_t)(UBRR0_VALUE >> 8);
	UBRR0L = (uint8_t)UBRR0_VALUE;

	UCSR0B = (1 << RXEN0) | (1 << TXEN0);
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

uint8_t uart0_poll_status(DisplayData *data)
{
	static char buffer[RX_BUFFER_SIZE];
	static uint8_t index = 0;
	char received;

	if (!(UCSR0A & (1 << RXC0))) {
		return 0;
	}

	received = (char)UDR0;

	/* 새 패킷 시작 */
	if (received == '<') {
		index = 0;
		buffer[index++] = received;
		return 0;
	}

	/* '<'가 오기 전의 불필요한 문자는 무시 */
	if (index == 0) {
		return 0;
	}

	/* 패킷이 너무 길면 폐기 */
	if (index >= RX_BUFFER_SIZE - 1) {
		index = 0;
		return 0;
	}

	buffer[index++] = received;

	/* 패킷 종료 후 해석 */
	if (received == '>') {
		buffer[index] = '\0';
		index = 0;

		return parse_status_packet(buffer, data);
	}

	return 0;
}