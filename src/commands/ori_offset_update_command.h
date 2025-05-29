#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include <memory>
#include "misc/encoder_value_integrator.h"

namespace MM {

class OriOffsetUpdater : public MotionCommandIF
{
public:
    OriOffsetUpdater(bool& offsetFlag);
    void execute() override;
    bool isFinished() const override;
    CommandResult getResult() override;

    // ONLY FOR DEBUG:
    void print() const override;
private:
    bool mStarted{false};
    bool mFinished{false};
    bool& mOffsetFlag;
};

} // namespace MM