#include "collision_avoidance_command.h"
#include "constants.h"
#include "utils/logging.h"

MM::CollisionAvoidanceCommand::CollisionAvoidanceCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& ir_left, uint16_t const& ir_right, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
mIrLeftR(ir_left),
mIrRightR(ir_right),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{}

void MM::CollisionAvoidanceCommand::execute()
{
    if( myWrappedCommandP.get() != nullptr )
    {
        if(mIrLeftR > 1300 || mIrRightR > 1000)
        {
            // avoid collision with wall in front
            mFinished = true;
            mLeftMotorVoltageR_mV = 0;
            mRightMotorVoltageR_mV = 0;
        }
        else
        {
            myWrappedCommandP->execute();
        }
    }
    else
    {
        mFinished = true;
    }
}

bool MM::CollisionAvoidanceCommand::isFinished() const
{
    if( !mFinished && myWrappedCommandP.get() != nullptr )
    {   
        return myWrappedCommandP->isFinished();
    }
    else
    {
        return true;
    }
}

MM::MotionCommandIF* MM::CollisionAvoidanceCommand::getWrappedObjectP()
{
    return myWrappedCommandP.get();
}

void MM::CollisionAvoidanceCommand::print() const
{
    myWrappedCommandP->print();
}