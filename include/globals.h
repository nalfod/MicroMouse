// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include "drv/pid/pidWrapper.h"

namespace MM {

struct Globals {
// Inputs =================================================
    uint16_t ir_left;
    uint16_t ir_frontleft;
    uint16_t ir_frontright;
    uint16_t ir_right;
    uint16_t currentBatteryVoltage;

// Targets =================================================
    // the "speed" which we wants to achieve
    uint16_t targetMotorVoltage;

// Outputs =================================================
    // the values which should be put on the motors
    int16_t leftMotorVoltage;
    int16_t rightMotorVoltage;
    

// Controllers =================================================
    PidWrapper myStraightMovementCtrl;

    Globals();
};

} // namespace MM

extern MM::Globals g;