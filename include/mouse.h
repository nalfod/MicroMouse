
// Micromouse course - evosoft
#pragma once

#include "hal/micromouse.h"
#include "drv/led/led.h"
#include "drv/motor/motor.h"
#include <ESP32Encoder.h>

namespace MM {

struct Mouse {
    LED dbg_green{PINS::DEBUG_LED_1};
    LED dbg_red{PINS::DEBUG_LED_2};
    LED ir_led1{PINS::IR_LED_1};
    LED ir_led2{PINS::IR_LED_2};
    LED ir_led3{PINS::IR_LED_3};
    LED ir_led4{PINS::IR_LED_4};

    Motor motor_right{MM::Motor::CLOCKWISE, PINS::MOTOR_1_A, PINS::MOTOR_1_B, MM::PINS::MOTOR_2_ENC_A, MM::PINS::MOTOR_2_ENC_B}; // RIGHT, BUT DOUBLE CHECK
    Motor motor_left{MM::Motor::COUNTER_CLOCKWISE, PINS::MOTOR_2_A, PINS::MOTOR_2_B, MM::PINS::MOTOR_1_ENC_A, MM::PINS::MOTOR_1_ENC_B}; // LEFT, BUT DOUBLE CHECK
};

} // namespace MM

extern MM::Mouse mouse;