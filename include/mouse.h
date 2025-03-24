
// Micromouse course - evosoft
#pragma once

#include "hal/micromouse.h"
#include "drv/led/led.h"

namespace MM {

struct Mouse {
    LED dbg_green{PINS::DEBUG_LED_1};
    LED dbg_red{PINS::DEBUG_LED_2};
    LED ir_led1{PINS::IR_LED_1};
    LED ir_led2{PINS::IR_LED_2};
    LED ir_led3{PINS::IR_LED_3};
    LED ir_led4{PINS::IR_LED_4};
};

} // namespace MM

extern MM::Mouse mouse;