#define F_CPU 16000000UL

#include <avr/io.h>
#include <util/delay.h>

#include "flood_types.h"
#include "risk_fsm.h"
#include "uart.h"

#define TEST_SCENARIO_COUNT 5

static const FloodInput test_scenarios[TEST_SCENARIO_COUNT] = {
    /* 수위, 상승속도, IR 5개, 정상여부, 오류코드 */

    { 10,   0, {0, 0, 0, 0, 0}, 1, 0 },  /* NORMAL */
    { 35,  10, {1, 0, 0, 0, 0}, 1, 0 },  /* CAUTION */
    { 90,  30, {1, 1, 0, 0, 0}, 1, 0 },  /* WARNING */
    {120, 100, {1, 1, 1, 0, 0}, 1, 0 },  /* DANGER */
    {  0,   0, {0, 0, 0, 0, 0}, 0, 1 }   /* ERROR */
};

static void read_test_input(FloodInput *input)
{
    static uint8_t index = 0;

    *input = test_scenarios[index];

    index++;

    if (index >= TEST_SCENARIO_COUNT) {
        index = 0;
    }
}

/*
 * 나중에 여기서 릴레이와 모터 드라이버 핀을 실제로 제어한다.
 * 지금은 저전압 LED 시험 전까지 비워 둔다.
 */

static void apply_control_output(const ControlOutput *out)
{
    (void)out;
}

int main(void)
{
    FloodInput input;
    ControlOutput output;
    RiskState state;
    uint16_t ttc_sec;

    uart1_init();

    while (1) {
        /* 1. 현재는 가짜 센서값 사용 */
        read_test_input(&input);

        /* 2. 위험 단계 판단 */
        state = update_risk_state(&input);

        /* 3. 위험 단계에 맞는 모터·전력 제어값 생성 */
        make_control_output(state, &output);

        /* 4. DANGER 수위까지 남은 예상 시간 계산 */
        ttc_sec = calculate_ttc_sec(
            input.water_level_mm,
            LEVEL_DANGER_MM,
            input.rise_rate_mm_min
        );

        /* 5. 나중에 릴레이·모터에 실제 출력 */
        apply_control_output(&output);

        /* 6. 두 번째 ATmega128으로 상태 전송 */
        uart1_send_status(
            input.water_level_mm,
            state,
            ttc_sec,
            &output,
            input.sensor_error
        );

        _delay_ms(1000);
    }
}