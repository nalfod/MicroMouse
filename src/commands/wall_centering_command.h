#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"
#include <memory>

namespace MM {

class WallCenteringCommand : public MotionCommandIF
{
public:
    WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    void execute() override;
    bool isFinished() const override;

    // ONLY FOR DEBUG:
    void print() const override;
    MotionCommandIF* getWrappedObjectP();
    int16_t getPidOutput() const;
    int32_t getCurrentError() const;
    uint16_t getFrontLeftSensor() { return mIrFrontLeftR; }
    uint16_t getFrontRightSensor() { return mIrFrontRightR; }
private:
    void executeWallCenteringControl();
    void executeCenteringUsingWallDistance();
    void executeCenteringUsingOrientation();
    bool isCenteringWithWallsPossible();

    std::unique_ptr<MotionCommandIF> myWrappedCommandP;

    // measure points
    uint16_t const& mIrFrontLeftR;
    uint16_t const& mIrFrontRightR;
    float const& myCurrentOriR_deg;

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    PidWrapper myCenteringPidForWalls;
    PidWrapper myCenteringPidForOrientation;


};

} // namespace MM
