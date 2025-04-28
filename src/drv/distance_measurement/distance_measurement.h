#pragma once
#include "../led/led.h"

namespace MM {

class DistanceMeasurement
{
    public:
        DistanceMeasurement(uint8_t led_pin, uint8_t phototransistor_pin, float eq_coeff, float eq_power);
        void turn_on_led();
        void turn_off_led();
        uint16_t get_current_phototransistor_value_mV();
        // Note: Not exact mm, just a good hint. Also, coeffs depends on the surface
        uint16_t get_current_distance_mm();
    private:
        void _update_phototransistor_value();
        LED _infra_led;
        uint8_t const _phototransistor_pin;
        uint16_t _current_phototransistor_value{0};

        // Trendline equation
        float _coeff{0.0};
        float _power{0.0};
};


}