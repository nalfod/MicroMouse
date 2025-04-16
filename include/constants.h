#pragma once
#include <chrono>

using namespace std::chrono_literals;

namespace CONSTS {

constexpr std::chrono::duration<uint32_t, std::micro> MAIN_CYCLE_TIME = 1ms;
constexpr std::chrono::duration<uint32_t, std::micro> DEBUG_CYCLE_TIME = 100ms;

constexpr float K_SPEED_FF = 1.540; // mV / (um/ms)
constexpr float K_BIAS_FF = 285.190; // mV
constexpr uint16_t ENCODER_RESOULTION = 288; // no unit
constexpr uint16_t WHEEL_DIAMETER_UM = 43800; // um

constexpr float K_SPEED_FF_REV = 644.7; // mV / (urev/ms)
constexpr float K_BIAS_FF_REV = 94.5; // mV

}