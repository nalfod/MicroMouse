// Micromouse course - evosoft
#pragma once

#include <cstdint>
#include <functional>
#include <chrono>
#include <Arduino.h>

using namespace std::chrono_literals;

namespace MM {

class Task {
using us_t = std::chrono::duration<uint32_t, std::micro>;
public:
  Task() = delete;
  Task(std::function<void()> func, us_t period, us_t start = 0us)
    : m_func(func), m_period(period), m_next(start > 0us ? start : period)
  { }

  void operator()() {
    auto now = std::chrono::microseconds(micros());
    // WARNING! This does not deal with overflow and that happens after ~70 minutes,
    // hence not an issue for this use-case.
    if (now >= m_next) {
        m_next += m_period;
        m_func();
#ifdef TASK_STATISTICS
        m_latency = m_next - now;
        if (m_latency < m_min_latency) {
          m_min_latency = m_latency;
        }
        if (m_latency > m_max_latency) {
          m_max_latency = m_latency;
        }
        auto completed = std::chrono::microseconds(micros());
        m_runtime = completed - now;
        if (m_runtime > m_max_runtime ) {
          m_max_runtime = m_runtime;
        }
#endif
    }
  }

#ifdef TASK_STATISTICS
  us_t get_latency() const { return m_latency; }
  us_t get_min_latency() const { return m_min_latency; }
  us_t get_max_latency() const { return m_max_latency; }
  us_t get_runtime() const { return m_runtime; }
  us_t get_max_runtime() const { return m_max_runtime; }
#endif

private:
  std::function<void()> m_func;
  us_t m_period;
  us_t m_next; 
#ifdef TASK_STATISTICS
  us_t m_latency = 0us;
  us_t m_min_latency = 70min;
  us_t m_max_latency = 0us;
  us_t m_runtime = 0us;
  us_t m_max_runtime = 0us;
#endif
};

} // namespace MM