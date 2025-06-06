#pragma once
#include "utils/logging.h"

namespace MM {

struct CommandResult
{
    CommandResult() = default;
    CommandResult(float distance_par_mm, float distance_per_mm, float angle_deg, float end_speed_mm_per_s = 0.0): 
        dist_traveled_parallel_mm(distance_par_mm), dist_traveled_perpendicular_mm(distance_per_mm), angle_turned_deg(angle_deg), end_speed_mm_per_s(end_speed_mm_per_s) { }
    
    float dist_traveled_parallel_mm{0.0};
    float dist_traveled_perpendicular_mm{0.0};
    float angle_turned_deg{0.0};
    float end_speed_mm_per_s{0.0};

    void print()
    {
        LOG_INFO("CMD_RES dist_par= %d dist_per= %d deg= %d end_speed= %d\n", static_cast<int>(dist_traveled_parallel_mm),
                                                                              static_cast<int>(dist_traveled_perpendicular_mm),
                                                                              static_cast<int>(angle_turned_deg),
                                                                              static_cast<int>(end_speed_mm_per_s) );
    }
};

} // namespace MM