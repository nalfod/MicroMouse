#pragma once
#include "utils/logging.h"

namespace MM {

struct CommandResult
{
    CommandResult() = default;
    CommandResult(float distance_mm, float angle_deg): distance_traveled_mm(distance_mm), angle_turned_deg(angle_deg) { }
    float distance_traveled_mm{0.0};
    float angle_turned_deg{0.0};

    void print()
    {
        LOG_INFO("CMD_RES dist= %d deg= %d\n", static_cast<int>(distance_traveled_mm), 
                                               static_cast<int>(angle_turned_deg) );
    }
};

} // namespace MM