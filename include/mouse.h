
// Micromouse course - evosoft
#pragma once

#include "hal/micromouse.h"
#include "drv/led/led.h"
#include "drv/motor/motor.h"
#include "drv/accelerometer/accerelometer.h"
#include "drv/distance_measurement/distance_measurement.h"

namespace MM {

struct Mouse {
    LED dbg_green{PINS::DEBUG_LED_1};
    LED dbg_red{PINS::DEBUG_LED_2};

    // TODO: Fine tune the filtering values if necessary
    DistanceMeasurement left_dist_measure{PINS::IR_LED_1, PINS::IR_1, 2610.3, -0.532, 0.9};
    DistanceMeasurement frontleft_dist_measure{PINS::IR_LED_2, PINS::IR_2, 2987.2, -0.55, 0.9};
    DistanceMeasurement frontright_dist_measure{PINS::IR_LED_3, PINS::IR_3, 2935.7, -0.539, 0.9};
    DistanceMeasurement right_dist_measure{PINS::IR_LED_4, PINS::IR_4, 1390.7, -0.457, 0.9};

    Motor motor_right{MM::Motor::CLOCKWISE, PINS::MOTOR_1_A, PINS::MOTOR_1_B, MM::PINS::MOTOR_2_ENC_A, MM::PINS::MOTOR_2_ENC_B}; // RIGHT, BUT DOUBLE CHECK
    Motor motor_left{MM::Motor::COUNTER_CLOCKWISE, PINS::MOTOR_2_A, PINS::MOTOR_2_B, MM::PINS::MOTOR_1_ENC_A, MM::PINS::MOTOR_1_ENC_B}; // LEFT, BUT DOUBLE CHECK
    Accelerometer accelerometer;
};

} // namespace MM

extern MM::Mouse mouse;