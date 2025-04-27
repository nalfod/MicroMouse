// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include "commands/motion_command_if.h"
#include <memory>
#include <queue>
#include "misc/mode_selector.h"

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
    int16_t leftMotorVoltage{0};
    int16_t rightMotorVoltage{0};

// Commands =================================================
    std::queue< std::unique_ptr<MotionCommandIF> > commandBuffer;

// Mode selector =================================================
    ModeSelector mode_selector{ir_left, ir_frontleft, ir_frontright, ir_right, CONSTS::MODES::MEASUREMENT};
};

} // namespace MM

extern MM::Globals g;