#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include <memory>
#include "misc/encoder_value_integrator.h"

namespace MM {

class StuckAvoidanceCommand : public MotionCommandIF
{
public:
    StuckAvoidanceCommand(std::unique_ptr<MotionCommandIF> commandToWrap, int64_t const& encoderValue1, int64_t const& encoderValue2, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    void execute() override;
    bool isFinished() const override;
    CommandResult getResult() override;

    // ONLY FOR DEBUG:
    void print() const override;
private:
    std::unique_ptr<MotionCommandIF> myWrappedCommandP;

    // measure points
    EncoderValueIntegrator myEncIntegrator1;
    EncoderValueIntegrator myEncIntegrator2;  

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    bool mStarted{false};
    bool mFinished{false};
};

} // namespace MM