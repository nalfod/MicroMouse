#pragma once
#include <chrono>

using namespace std::chrono_literals;

namespace CONSTS {

constexpr std::chrono::duration<uint32_t, std::micro> MAIN_CYCLE_TIME = 2ms;
constexpr std::chrono::duration<uint32_t, std::micro> DEBUG_CYCLE_TIME = 100ms;

}