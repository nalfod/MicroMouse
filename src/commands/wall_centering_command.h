#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"
#include <memory>
#include "movement_stabilizers/movement_stabilizer_if.h"

namespace MM {

class WallCenteringCommand : public MotionCommandIF
{
public:
    WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, std::vector<std::unique_ptr<MM::MovementStabilizerIF>>&& stabilizers, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    void execute() override;
    bool isFinished() const override;
    CommandResult getResult() override;

    // ONLY FOR DEBUG:
    void print() const override;
private:
    void executeWallCenteringControl();

    std::unique_ptr<MotionCommandIF> myWrappedCommandP;
    std::vector<std::unique_ptr<MM::MovementStabilizerIF>> myStabilizers;

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    // state flags
    bool mStarted{false};
};

} // namespace MM
