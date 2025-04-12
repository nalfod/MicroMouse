// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include "commands/motion_command_if.h"

namespace MM {

struct Globals {
// Inputs =================================================
    uint16_t ir_left;
    uint16_t ir_frontleft;
    uint16_t ir_frontright;
    uint16_t ir_right;
    uint16_t currentBatteryVoltage;
    int64_t leftEncoderValue;
    int64_t rightEncoderValue;

// Outputs =================================================
    // the values which should be put on the motors
    int16_t leftMotorVoltage;
    int16_t rightMotorVoltage;

// Commands =================================================

    MotionCommandIF* currentCommand; 
};

} // namespace MM

extern MM::Globals g;