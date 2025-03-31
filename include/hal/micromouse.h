// Micromouse course - evosoft
#pragma once

namespace MM {
namespace PINS {

// Debug LEDS
constexpr int DEBUG_LED_1 = 1;
constexpr int DEBUG_LED_2 = 2;

// Motors
constexpr int MOTOR_DRV_EN = 41;
constexpr int MOTOR_DRV_FAULT = 40;
// RIGHT
constexpr int MOTOR_1_A = 11;
constexpr int MOTOR_1_B = 13;
//LEFT
constexpr int MOTOR_2_A = 12;
constexpr int MOTOR_2_B = 14;

constexpr int MOTOR_1_ENC_A = 9;
constexpr int MOTOR_1_ENC_B = 10;
constexpr int MOTOR_2_ENC_A = 48;
constexpr int MOTOR_2_ENC_B = 42;

// Infra distance sensors
constexpr int IR_LED_1 = 18;
constexpr int IR_LED_2 = 17;
constexpr int IR_LED_3 = 15;
constexpr int IR_LED_4 = 16;

constexpr int IR_1 = 5;
constexpr int IR_2 = 7;
constexpr int IR_3 = 4;
constexpr int IR_4 = 6;

// IMU
constexpr int IMU_SDA = 21;
constexpr int IMU_SCL = 47;
constexpr int IMU_INT = 38;

// Battery sensor
constexpr int BATT = 8;

} // namespace PINS
} // namespace MM