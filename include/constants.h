#pragma once
#include <chrono>
#include <array>

using namespace std::chrono_literals;

namespace CONSTS {

constexpr float EPSILON = 1e-5f; // for float comparasion

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
constexpr float WHEEL_BASE_MM = 100; // TODO: MEASURE THIS

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

constexpr float HALF_CELL_DISTANCE_MM = 90.00;
constexpr float HOME_POSITION_IN_CELL_MM = 31.5; // an absolute value, the sign is orientation dependent

// MODE SELECTION
enum MODES
{
    IDLE = 0,
    SPEED_RUN = 1,
    DISCOVERY = 2,
    MEASUREMENT = 3,
    MEASUREMENT_SNAPSHOT = 4,
    TESTING = 5
};

constexpr uint16_t MODE_SPEED_RUN_ACTIVATION_LIMIT = 1;

constexpr uint16_t MODE_SIGNAL_THRESHOLD_MV = 3900;
constexpr uint16_t MODE_SIGNAL_HOLD_TIME_MS = 2000;

constexpr uint16_t COLLISION_AVOIDANCE_DIST_MM = 50;


/////////////////////////
// DIRECTION MANIPULATION
/////////////////////////
enum Direction
{
    NORTH   = 1,
    EAST    = 2,
    SOUTH   = 4,
    WEST    = 8,
    UNKNOWN = 16
};

inline
Direction getDirectionAfterRotation(Direction currentDirection, float rotationAngle) {
    if (rotationAngle > 89.9f && rotationAngle < 90.1f) {
        switch (currentDirection) {
            case Direction::NORTH: return Direction::EAST;
            case Direction::EAST: return Direction::SOUTH;
            case Direction::SOUTH: return Direction::WEST;
            case Direction::WEST: return Direction::NORTH;
            default: return Direction::UNKNOWN;
        }
    } else if ( ( rotationAngle > 179.9f && rotationAngle < 180.1f ) ||
                ( rotationAngle > -180.1f && rotationAngle < -179.9f) ) {
        switch (currentDirection) {
            case Direction::NORTH: return Direction::SOUTH;
            case Direction::EAST: return Direction::WEST;
            case Direction::SOUTH: return Direction::NORTH;
            case Direction::WEST: return Direction::EAST;
            default: return Direction::UNKNOWN;
        }
    } else if (rotationAngle > -90.1f && rotationAngle < -89.9f) {
        switch (currentDirection) {
            case Direction::NORTH: return Direction::WEST;
            case Direction::EAST: return Direction::NORTH;
            case Direction::SOUTH: return Direction::EAST;
            case Direction::WEST: return Direction::SOUTH;
            default: return Direction::UNKNOWN;
        }
    } else {
        return Direction::UNKNOWN; // Invalid rotation angle
    }
}

/////////////////////////
// COMMAND CREATION /////
/////////////////////////
inline
int getRotationAngle(Direction source, Direction destination) {
    if (source == destination) {
        return 0; // No rotation needed
    }

    switch (source) {
        case Direction::NORTH:
            switch (destination) {
                case Direction::EAST: return 90;
                case Direction::SOUTH: return 180;
                case Direction::WEST: return -90;
                default: break;
            }
            break;

        case Direction::EAST:
            switch (destination) {
                case Direction::SOUTH: return 90;
                case Direction::WEST: return 180;
                case Direction::NORTH: return -90;
                default: break;
            }
            break;

        case Direction::SOUTH:
            switch (destination) {
                case Direction::WEST: return 90;
                case Direction::NORTH: return 180;
                case Direction::EAST: return -90;
                default: break;
            }
            break;

        case Direction::WEST:
            switch (destination) {
                case Direction::NORTH: return 90;
                case Direction::EAST: return 180;
                case Direction::SOUTH: return -90;
                default: break;
            }
            break;

        default:
            break;
    }

    return -1; // Invalid rotation (e.g., UNKNOWN direction)
}

inline
float adjustAngleToAlignGridDirection( float currentOrientation )
{
    if( -30.0 < currentOrientation && currentOrientation < 30 )
    {
        currentOrientation = 0.0;
    }
    else if( 60.0 < currentOrientation && currentOrientation < 120.0 )
    {
        currentOrientation = 90.0;
    }
    else if( -120.0 < currentOrientation && currentOrientation < -60.0 )
    {
        currentOrientation = -90.0;
    }
    else if( -210.0 < currentOrientation && currentOrientation < -150.0 )
    {
        currentOrientation = -179.9999;
    }
    else if( 150.0 < currentOrientation && currentOrientation < 210.0 )
    {
        currentOrientation = 179.9999;
    }

    return currentOrientation;
}

inline
float modifyAngleIfCircleOverflow( float input_deg )
{
    if( input_deg >= 180.0 )
    {
        input_deg -= 360.0;
    }
    else if( input_deg <= -180.0 )
    {
        input_deg += 360.0;
    }
    return input_deg;
}

}