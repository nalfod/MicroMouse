#pragma once
#include <cstdint>
#include "constants.h"

namespace MM {

class SignalValue
{
    public:
        SignalValue(uint16_t const& signalR);
        bool get_value();
    private:
        uint16_t const& _ir_signalR;
        unsigned long _start_time{0};
};

class ModeSelector
{
    public:
        ModeSelector(uint16_t const& ir_left, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, uint16_t const& ir_right);

        CONSTS::MODES get_current_mode();
        bool is_mode_just_changed();
        void update();

    private:
        SignalValue _left_signal;
        SignalValue _front_left_signal;
        SignalValue _front_right_signal;
        SignalValue _right_signal;

        CONSTS::MODES _current_mode{CONSTS::MODES::IDLE};

        bool _is_ready_for_new_mode{false};
        bool _is_mode_just_changed{false};
};

} // namespace MM    