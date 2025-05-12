#pragma once
#include <chrono>
#include <array>

using namespace std::chrono_literals;

namespace CONSTS {

constexpr std::chrono::duration<uint32_t, std::micro> MAIN_CYCLE_TIME = 1ms;
constexpr std::chrono::duration<uint32_t, std::micro> DIST_MEASURE_CYCLE_TIME = 10ms;
constexpr std::chrono::duration<uint32_t, std::micro> BATTERY_CHECK_TIME = 1000ms;
constexpr std::chrono::duration<uint32_t, std::micro> DEBUG_CYCLE_TIME = 100ms;

constexpr std::chrono::duration<uint32_t, std::micro> IDLE_BLINK_TIME = 1000ms;
constexpr std::chrono::duration<uint32_t, std::micro> SPEED_RUN_BLINK_TIME = 100ms;
constexpr std::chrono::duration<uint32_t, std::micro> DISCOVERY_BLINK_TIME = 500ms;
constexpr std::chrono::duration<uint32_t, std::micro> MEASUREMENT_BLINK_TIME = 750ms;

// Encoder and wheel data
constexpr uint16_t ENCODER_RESOULTION = 288; // no unit
constexpr float WHEEL_DIAMETER_MM = 43.8;

// speed - voltage function coeeffs in case of linear motion
constexpr float K_SPEED_FF = 1.540; // mV / (mm/s)
constexpr float K_BIAS_FF = 285.190; // mV the absolute minimu theretical value for the motor to start

// speed - voltage function coeeffs in case of rotation movement
constexpr float K_SPEED_FF_REV = 644.7; // mV / (urev/ms)
constexpr float K_BIAS_FF_REV = 94.5; // mV
constexpr float K_BIAS_PID_REV = K_BIAS_FF * 1.5;

// FIXME: this is quite small beacuse otherwise the wall in front of would cause to trigger the wall centering too
// somehow avoid it maybe? by passing the left and right sensors to the wall centering class as well?
constexpr uint16_t WALL_DISTANCE_LIMIT_FOR_CENTERING_MM = 90;
constexpr uint16_t WALL_DISTANCE_MID_FOR_CENTERING_MM = 67;

constexpr uint16_t CELL_DISTANCE_MM = 180;

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

enum Direction
{
    NORTH   = 1,
    EAST    = 2,
    SOUTH   = 4,
    WEST    = 8,
    UNKNOWN = 16
};

constexpr std::array<std::pair<Direction, std::array<std::pair<float, Direction>, 3>>, 4> DIRECTION_MAP = {{
    { Direction::NORTH, {{
        {90.0f, Direction::EAST},
        {180.0f, Direction::SOUTH},
        {-90.0f, Direction::WEST}
    }}},
    { Direction::EAST, {{
        {90.0f, Direction::SOUTH},
        {180.0f, Direction::WEST},
        {-90.0f, Direction::NORTH}
    }}},
    { Direction::SOUTH, {{
        {90.0f, Direction::WEST},
        {180.0f, Direction::NORTH},
        {-90.0f, Direction::EAST}
    }}},
    { Direction::WEST, {{
        {90.0f, Direction::NORTH},
        {180.0f, Direction::EAST},
        {-90.0f, Direction::SOUTH}
    }}}
}};


}