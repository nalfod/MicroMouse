// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include "commands/command_executer.h"
#include <memory>
#include <queue>
#include "misc/mode_selector.h"
#include "maze/location_controller.h"
#include "maze/cell_position.h"

namespace MM {

struct Globals {
// Inputs =================================================
    // raw IR values
    uint16_t ir_left{0};
    uint16_t ir_frontleft{0};
    uint16_t ir_frontright{0};
    uint16_t ir_right{0};
    // calculated distances (not safe to use for too precise calculations!)
    uint16_t dist_left_mm{0};
    uint16_t dist_frontleft_mm{0};
    uint16_t dist_frontright_mm{0};
    uint16_t dist_right_mm{0};

    uint16_t currentBatteryVoltage{0};
    int64_t leftEncoderValue{0};
    int64_t rightEncoderValue{0};
    float currentOrientation{0};

// Outputs =================================================
    int16_t leftMotorVoltage{0};
    int16_t rightMotorVoltage{0};

// Position and location control
    CellPosition currentCellPosition{-35.0, 0.0, CONSTS::Direction::NORTH};
    LocationController locController{8, currentCellPosition, dist_left_mm, dist_right_mm, dist_frontleft_mm, dist_frontright_mm};

// Mode selector =================================================
    ModeSelector mode_selector{ir_left, ir_frontleft, ir_frontright, ir_right, CONSTS::MODES::DISCOVERY};

// Commands =================================================
    CommandExecuter commandExecuter{currentCellPosition, dist_left_mm, dist_right_mm, dist_frontleft_mm, dist_frontright_mm, 
                                    leftEncoderValue, rightEncoderValue, currentOrientation, leftMotorVoltage, rightMotorVoltage};
};

} // namespace MM

extern MM::Globals g;