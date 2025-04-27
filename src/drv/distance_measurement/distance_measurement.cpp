#include "distance_measurement.h"


MM::DistanceMeasurement::DistanceMeasurement(uint8_t led_pin, uint8_t phototransistor_pin):
_infra_led(led_pin),
_phototransistor_pin(phototransistor_pin)
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
    _current_phototransistor_value = analogRead( _phototransistor_pin );
    return _current_phototransistor_value;
}


uint16_t MM::DistanceMeasurement::get_current_distance_mm()
{
    // not implemented yet
    return 0;
}