#ifndef RISK_FSM_H
#define RISK_FSM_H

#include "flood_types.h"

/* 실제 센서 설치 높이를 측정한 뒤 이 숫자를 수정 */
#define LEVEL_CAUTION_MM   30U
#define LEVEL_WARNING_MM   80U
#define LEVEL_DANGER_MM   140U

#define TTC_WARNING_SEC   180U
#define TTC_DANGER_SEC     60U
#define TTC_NONE        65535U

uint16_t calculate_ttc_sec(uint16_t current_mm,
uint16_t target_mm,
int16_t rise_rate_mm_min);

RiskState update_risk_state(const FloodInput *input);

void make_control_output(RiskState state, ControlOutput *out);

#endif