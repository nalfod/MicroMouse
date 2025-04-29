#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"
#include <memory>

namespace MM {

class WallCenteringCommand : public MotionCommandIF
{
public:
    WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& dist_frontleft, uint16_t const& dist_frontright, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    void execute() override;
    bool isFinished() const override;

    // ONLY FOR DEBUG:
    void print() const override;
    MotionCommandIF* getWrappedObjectP();
    int16_t getPidOutput() const;
    int32_t getCurrentError() const;
    uint16_t getFrontLeftSensor() { return mDistFrontLeftR_mm; }
    uint16_t getFrontRightSensor() { return mDistFrontRightR_mm; }
private:
    void executeWallCenteringControl();
    void executeCenteringUsingWallDistance();
    void executeCenteringUsingOrientation();
    bool isCenteringWithWallsPossible() const;
    double shiftOrientationValueRespectedToTarget(float currentOrientation);

    std::unique_ptr<MotionCommandIF> myWrappedCommandP;

    // measure points
    uint16_t const& mDistFrontLeftR_mm;
    uint16_t const& mDistFrontRightR_mm;
    float const& myCurrentOriR_deg;

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    PidWrapper myCenteringPidForWalls;
    PidWrapper myCenteringPidForOrientation;

    // state flags
    bool mStarted{false};
};

} // namespace MM
