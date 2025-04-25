// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include "commands/motion_command_if.h"
#include <memory>
#include <queue>

namespace MM {

struct Globals {
// Inputs =================================================
    uint16_t ir_left{0};
    uint16_t ir_frontleft{0};
    uint16_t ir_frontright{0};
    uint16_t ir_right{0};
    uint16_t currentBatteryVoltage{0};
    int64_t leftEncoderValue{0};
    int64_t rightEncoderValue{0};
    float currentOrientation{0};

// Outputs =================================================
    // the values which should be put on the motors
    int16_t leftMotorVoltage{0};
    int16_t rightMotorVoltage{0};

// Commands =================================================

    std::queue< std::unique_ptr<MotionCommandIF> > commandBuffer;
};

} // namespace MM

extern MM::Globals g;