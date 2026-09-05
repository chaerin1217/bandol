#define F_CPU 16000000UL

#include <util/delay.h>

#include "uart_rx.h"

static void update_display(const DisplayData *data)
{
    /*
     * LCD 정하고 여기에 표시 코드
     *
     * 예시:
     *
     * NORMAL    10mm
     * PUMP:0  PWR:0
     *
     * DANGER   120mm
     * PUMP:100 PWR:3
     *
     * communication_ok == 0 이면:
     * "COMM ERROR" 표시
     */
    (void)data;
}

int main(void)
{
    DisplayData display_data = {0};
    uint16_t no_packet_time_ms = 0;

    uart0_init();

    while (1) {
        /* 메인 MCU에서 완성된 상태 패킷을 받았을 때 */
        if (uart0_poll_status(&display_data)) {
            no_packet_time_ms = 0;
            update_display(&display_data);
        }
        else {
            _delay_ms(10);

            if (no_packet_time_ms < 2000) {
                no_packet_time_ms += 10;
            }

            /* 2초 동안 수신이 없으면 통신 오류 */
            if (no_packet_time_ms >= 2000) {
                display_data.communication_ok = 0;
                update_display(&display_data);
            }
        }
    }
}