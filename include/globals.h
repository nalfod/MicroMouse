// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include "drv/pid/pidWrapper.h"

#include "commands/linear_travel_command.h"

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

// Targets =================================================
    // the "speed" which we wants to achieve
    uint16_t targetMotorVoltage;

// Outputs =================================================
    // the values which should be put on the motors
    int16_t leftMotorVoltage;
    int16_t rightMotorVoltage;
    

// Controllers =================================================
    PidWrapper myStraightMovementCtrl;

// Commands =================================================

    LinearTravelCommand* currentCommand; 


// Constructor
    Globals();
};

} // namespace MM

extern MM::Globals g;