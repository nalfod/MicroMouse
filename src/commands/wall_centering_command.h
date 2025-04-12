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
    bool isFinished() override;

    // ONLY FOR DEBUG:
    MotionCommandIF* getWrappedObjectP();
private:
    MotionCommandIF* myWrappedCommandP;

    // measure points
    int16_t const& mIrFrontLeftR;
    int16_t const& mIrFrontRightR;   

    // controlled units
    int16_t& mLeftMotorVoltageR_mV;
    int16_t& mRightMotorVoltageR_mV;

    PidWrapper myCenteringPidCtrl;


};

} // namespace MM
