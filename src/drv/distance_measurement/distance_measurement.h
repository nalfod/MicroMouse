#pragma once
#include "../led/led.h"

namespace MM {

class DistanceMeasurement
{
    public:
        DistanceMeasurement(uint8_t led_pin, uint8_t phototransistor_pin);
        void turn_on_led();
        void turn_off_led();
        uint16_t get_current_phototransistor_value_mV();
        uint16_t get_current_distance_mm();
    private:
        LED _infra_led;
        uint8_t const _phototransistor_pin;
        uint16_t _current_phototransistor_value{0};
};


}