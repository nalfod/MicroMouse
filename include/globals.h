// Micromouse course - evosoft
#pragma once

#include <cstdint>

namespace MM {

struct Globals {
// Inputs =================================================
    uint16_t ir_left;
    uint16_t ir_frontleft;
    uint16_t ir_frontright;
    uint16_t ir_right;

    int8_t pwmPercentLeft;
    int8_t pwmPercentRight;

    uint16_t currentBatteryVoltage;
};

} // namespace MM

extern MM::Globals g;