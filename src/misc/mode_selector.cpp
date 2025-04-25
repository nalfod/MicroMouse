#include "mode_selector.h"
#include "Arduino.h" // for millis()

MM::SignalValue::SignalValue(uint16_t const& signalR):
_ir_signalR(signalR)
{

}

bool MM::SignalValue::get_value()
{
    unsigned long now = millis();

    if ( _ir_signalR > CONSTS::MODE_SIGNAL_THRESHOLD_MV ) 
    {
        if ( _start_time == 0 ) 
        {
            _start_time = now; // Start timing if signal exceeds threshold
        } 
        else if ( now - _start_time >= CONSTS::MODE_SIGNAL_HOLD_TIME_MS ) 
        {
            return true; // Signal has exceeded threshold for the required time
        }
    } 
    else 
    {
        _start_time = 0; // Reset timing if signal drops below threshold
    }

    return false;
}


MM::ModeSelector::ModeSelector(uint16_t const& ir_left, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, uint16_t const& ir_right):
_left_signal(ir_left),
_front_left_signal(ir_frontleft),
_front_right_signal(ir_frontright),
_right_signal(ir_right)
{

}

inline
CONSTS::MODES MM::ModeSelector::get_current_mode()
{
    return _current_mode;
}

void MM::ModeSelector::update()
{
    bool left_signal = _left_signal.get_value();
    bool front_left_signal = _front_left_signal.get_value();
    bool front_right_signal = _front_right_signal.get_value();
    bool right_signal = _right_signal.get_value();

    switch (_current_mode)
    {
        case CONSTS::MODES::IDLE:
            if( left_signal )            _current_mode = CONSTS::MODES::SPEED_RUN;
            else if( right_signal )      _current_mode = CONSTS::MODES::DISCOVERY;
            else if( front_left_signal ) _current_mode = CONSTS::MODES::MEASUREMENT;
            else                         _current_mode = CONSTS::MODES::IDLE;
            break;

        case CONSTS::MODES::SPEED_RUN:
        case CONSTS::MODES::DISCOVERY:
            if( front_left_signal && front_right_signal ) _current_mode = CONSTS::MODES::IDLE;
            break;

        case CONSTS::MODES::MEASUREMENT:
            if( front_left_signal && front_right_signal ) _current_mode = CONSTS::MODES::IDLE;
            else if( left_signal || front_left_signal || front_right_signal || right_signal ) _current_mode = CONSTS::MODES::MEASUREMENT_SNAPSHOT;
            break;

        case CONSTS::MODES::MEASUREMENT_SNAPSHOT:
            if( !(left_signal || front_left_signal || front_right_signal || right_signal) ) _current_mode = CONSTS::MODES::MEASUREMENT;
            break;
        
        default:
            break;
    }
}

