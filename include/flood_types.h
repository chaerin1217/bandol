#ifndef FLOOD_TYPES_H
#define FLOOD_TYPES_H

#include <stdint.h>

typedef enum {
	RISK_NORMAL = 0,
	RISK_CAUTION,
	RISK_WARNING,
	RISK_DANGER,
	RISK_ERROR
} RiskState;

typedef struct {
	uint16_t water_level_mm; // 현재 수위
	int16_t rise_rate_mm_min; // 수위 상승 속도
	uint8_t ir_level[5]; // IR 센서 5개 상태
	uint8_t sensor_valid; // 정상여부 판단
	uint8_t sensor_error; // 세부 오류
} FloodInput;

typedef struct {
	uint8_t power_stage; // 0~3
	uint8_t pump_enable; // 0: OFF, 1: ON
	uint8_t pump_pwm; // 0~100%
	uint8_t emergency; // 정상 판단
} ControlOutput;

#endif