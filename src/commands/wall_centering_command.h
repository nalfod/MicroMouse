#pragma once
#include "motion_command_if.h"
#include <cstdint>
#include "drv/pid/pidWrapper.h"

namespace MM {

class WallCenteringCommand : public MotionCommandIF
{
public:
    WallCenteringCommand(MotionCommandIF* commandToWrap, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV);
    void execute() override;
    bool isFinished() const override;

    // ONLY FOR DEBUG:
    MotionCommandIF* getWrappedObjectP();
    int16_t getPidOutput() const;
    int32_t getCurrentError() const;
    uint16_t getFrontLeftSensor() { return mIrFrontLeftR; }
    uint16_t getFrontRightSensor() { return mIrFrontRightR; }
private:
    MotionCommandIF* myWrappedCommandP;

    // measure points (OR STORE AS UINT??)
    uint16_t const& mIrFrontLeftR;
    uint16_t const& mIrFrontRightR;   

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    PidWrapper myCenteringPidCtrl;


};

} // namespace MM
