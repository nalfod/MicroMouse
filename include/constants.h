#pragma once
#include <chrono>

using namespace std::chrono_literals;

namespace CONSTS {

constexpr std::chrono::duration<uint32_t, std::micro> MAIN_CYCLE_TIME = 1ms;
constexpr std::chrono::duration<uint32_t, std::micro> DEBUG_CYCLE_TIME = 100ms;

constexpr std::chrono::duration<uint32_t, std::micro> IDLE_BLINK_TIME = 1000ms;
constexpr std::chrono::duration<uint32_t, std::micro> SPEED_RUN_BLINK_TIME = 100ms;
constexpr std::chrono::duration<uint32_t, std::micro> DISCOVERY_BLINK_TIME = 500ms;
constexpr std::chrono::duration<uint32_t, std::micro> MEASUREMENT_BLINK_TIME = 750ms;

// Encoder and wheel data
constexpr uint16_t ENCODER_RESOULTION = 288; // no unit
constexpr uint16_t WHEEL_DIAMETER_UM = 43800; // um

// speed - voltage function coeeffs in case of linear motion
constexpr float K_SPEED_FF = 1.540; // mV / (um/ms)
constexpr float K_BIAS_FF = 285.190; // mV the absolute minimu theretical value for the motor to start

// speed - voltage function coeeffs in case of rotation movement
constexpr float K_SPEED_FF_REV = 644.7; // mV / (urev/ms)
constexpr float K_BIAS_FF_REV = 94.5; // mV
constexpr float K_BIAS_PID_REV = K_BIAS_FF * 1.5;

// FIXME: this is quite small beacuse otherwise the wall in front of would cause to trigger the wall centering too
// somehow avoid it maybe? by passing the left and right sensors to the wall centering class as well?
constexpr uint16_t WALL_DISTANCE_LIMIT_FOR_CENTERING_MM = 90;

// MODE SELECTION
enum MODES
{
    IDLE = 0,
    SPEED_RUN = 1,
    DISCOVERY = 2,
    MEASUREMENT = 3,
    MEASUREMENT_SNAPSHOT = 4
};

constexpr uint16_t MODE_SIGNAL_THRESHOLD_MV = 3900;
constexpr uint16_t MODE_SIGNAL_HOLD_TIME_MS = 2000;

constexpr uint16_t COLLISION_AVOIDANCE_DIST_MM = 50;

}