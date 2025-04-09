// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include <PID_v1.h>
#include <ESP32Encoder.h>

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


    // Straight movement
    double errorBetweenSides = 0;
    double targetBetweenSides = 0;
    double output = 0;

    PID myStraightMovementCtrl;

    ESP32Encoder encoder1;
    ESP32Encoder encoder2;

    Globals(): myStraightMovementCtrl(&errorBetweenSides, &output , &targetBetweenSides,
                                      0.005, 0, 0, DIRECT) {}
};

} // namespace MM

extern MM::Globals g;