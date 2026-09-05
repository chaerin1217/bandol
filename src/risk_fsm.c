#include "risk_fsm.h"

uint16_t calculate_ttc_sec(uint16_t current_mm,
uint16_t target_mm,
int16_t rise_rate_mm_min)
{
	uint32_t ttc;

	/* 수위가 내려가거나 멈췄다면 도달 시간 계산 안 함 */
	if (rise_rate_mm_min <= 0) {
		return TTC_NONE;
	}

	/* 이미 목표 수위 이상이면 0초 */
	if (current_mm >= target_mm) {
		return 0;
	}

	ttc = ((uint32_t)(target_mm - current_mm) * 60U)
	/ (uint16_t)rise_rate_mm_min;

	if (ttc > TTC_NONE) {
		return TTC_NONE;
	}

	return (uint16_t)ttc;
}

RiskState update_risk_state(const FloodInput *input)
{
	uint16_t ttc_warning;
	uint16_t ttc_danger;

	if (input == 0) {
		return RISK_ERROR;
	}

	/* 센서 데이터가 신뢰할 수 없으면 안전 우선 */
	if (input->sensor_valid == 0 || input->sensor_error != 0) {
		return RISK_ERROR;
	}

	ttc_warning = calculate_ttc_sec(
	input->water_level_mm,
	LEVEL_WARNING_MM,
	input->rise_rate_mm_min
	);

	ttc_danger = calculate_ttc_sec(
	input->water_level_mm,
	LEVEL_DANGER_MM,
	input->rise_rate_mm_min
	);

	/* 높은 위치 IR 센서 감지 또는 위험 수위/TTC */
	if (input->ir_level[2] ||
	input->ir_level[3] ||
	input->ir_level[4] ||
	input->water_level_mm >= LEVEL_DANGER_MM ||
	ttc_danger <= TTC_DANGER_SEC) {
		return RISK_DANGER;
	}

	/* 2번째 IR 센서 감지 또는 경고 수위/TTC */
	if (input->ir_level[1] ||
	input->water_level_mm >= LEVEL_WARNING_MM ||
	ttc_warning <= TTC_WARNING_SEC) {
		return RISK_WARNING;
	}

	/* 첫 번째 IR 센서 감지 또는 주의 수위 */
	if (input->ir_level[0] ||
	input->water_level_mm >= LEVEL_CAUTION_MM) {
		return RISK_CAUTION;
	}

	return RISK_NORMAL;
}

void make_control_output(RiskState state, ControlOutput *out)
{
	out->power_stage = 0;
	out->pump_enable = 0;
	out->pump_pwm = 0;
	out->emergency = 0;

	switch (state) {
		case RISK_NORMAL:
		break;

		case RISK_CAUTION:
		/* LCD/LED 경고만 표시 */
		break;

		case RISK_WARNING:
		out->power_stage = 1;
		out->pump_enable = 1;
		out->pump_pwm = 70;
		break;

		case RISK_DANGER:
		out->power_stage = 3;
		out->pump_enable = 1;
		out->pump_pwm = 100;
		out->emergency = 1;
		break;

		case RISK_ERROR:
		/* 센서 오류면 보수적으로 동작 */
		out->power_stage = 3;
		out->pump_enable = 1;
		out->pump_pwm = 100;
		out->emergency = 1;
		break;
	}
}