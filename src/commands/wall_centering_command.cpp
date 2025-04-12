#include "wall_centering_command.h"

MM::WallCenteringCommand::WallCenteringCommand(MotionCommandIF* commandToWrap, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(commandToWrap),
mIrFrontLeftR(ir_frontleft),
mIrFrontRightR(ir_frontright),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV),
myCenteringPidCtrl(0.05, 0, 0) // TODO: Maybe the tuning should be more sofisticated!!
{
    myCenteringPidCtrl.setTarget(0);
    myCenteringPidCtrl.init(1, AUTOMATIC, -500, 500); // TODO: Maybe the tuning should be more sofisticated!!
}

void MM::WallCenteringCommand::execute()
{
    myWrappedCommandP->execute();
    if( !myWrappedCommandP->isFinished() )
    {
        myCenteringPidCtrl.compute( static_cast<double>( mIrFrontLeftR - mIrFrontRightR ) );
        mLeftMotorVoltageR_mV = mLeftMotorVoltageR_mV - myCenteringPidCtrl.getOuput();
        mRightMotorVoltageR_mV = mRightMotorVoltageR_mV + myCenteringPidCtrl.getOuput();
    }
}

bool MM::WallCenteringCommand::isFinished()
{
    return myWrappedCommandP->isFinished();
}

MM::MotionCommandIF* MM::WallCenteringCommand::getWrappedObjectP()
{
    return myWrappedCommandP;
}