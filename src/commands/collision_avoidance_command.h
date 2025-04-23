#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include <memory>

namespace MM {

class CollisionAvoidanceCommand : public MotionCommandIF
{
public:
    CollisionAvoidanceCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& ir_left, uint16_t const& ir_right, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    void execute() override;
    bool isFinished() const override;

    // ONLY FOR DEBUG:
    void print() const override;
    MotionCommandIF* getWrappedObjectP();
private:
    std::unique_ptr<MotionCommandIF> myWrappedCommandP;

    // measure points
    uint16_t const& mIrLeftR;
    uint16_t const& mIrRightR;   

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    bool mFinished{false};
};

} // namespace MM
