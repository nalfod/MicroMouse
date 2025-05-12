#include "distance_measurement.h"


MM::DistanceMeasurement::DistanceMeasurement(uint8_t led_pin, uint8_t phototransistor_pin, float eq_coeff, float eq_power, float filterint_factor):
_infra_led(led_pin),
_phototransistor_pin(phototransistor_pin),
_coeff(eq_coeff),
_power(eq_power),
_filtering_factor(filterint_factor)
{

}

void MM::DistanceMeasurement::turn_on_led()
{
    _infra_led.on();
}

void MM::DistanceMeasurement::turn_off_led()
{
    _infra_led.off();
}

uint16_t MM::DistanceMeasurement::get_current_phototransistor_value_mV()
{
    _update_phototransistor_value();
    return _current_phototransistor_value;
}


uint16_t MM::DistanceMeasurement::get_current_distance_mm()
{
    _update_phototransistor_value();
    return static_cast<uint16_t>( _coeff * pow( static_cast<float>(_current_phototransistor_value),  _power ) );
}

void MM::DistanceMeasurement::_update_phototransistor_value()
{
    _current_phototransistor_value += _filtering_factor * ( analogRead( _phototransistor_pin ) - _current_phototransistor_value );
}