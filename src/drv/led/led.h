// Micromouse course - evosoft
#pragma once

#include <Arduino.h>

namespace MM {

class LED {
public:
    LED(uint8_t pin) : m_pin{pin}, m_state{}
    {
        pinMode(m_pin, OUTPUT);
        digitalWrite(m_pin, m_state);
    }

    inline void on()
    {
        m_state = true;
        digitalWrite(m_pin, m_state);
    }

    inline void off()
    {
        m_state = false;
        digitalWrite(m_pin, m_state);
    }

    inline void toggle()
    {
        m_state = !m_state;
        digitalWrite(m_pin, m_state);
    }

private:
    uint8_t m_pin;
    bool m_state;
};

} // namespace MM