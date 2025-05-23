#include "collision_avoidance_command.h"
#include "constants.h"
#include "utils/logging.h"

MM::CollisionAvoidanceCommand::CollisionAvoidanceCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& dist_left, uint16_t const& dist_right, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
mDistLeftR_mm(dist_left),
mDistRightR_mm(dist_right),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{}

void MM::CollisionAvoidanceCommand::execute()
{
    if( myWrappedCommandP.get() != nullptr )
    {
        // FIXME: this interfere with the mode selector logic now, mode selection will automatically kills the first command!
        if( mDistLeftR_mm < CONSTS::COLLISION_AVOIDANCE_DIST_MM || 
            mDistRightR_mm < CONSTS::COLLISION_AVOIDANCE_DIST_MM )
        {
            // avoid collision with wall in front
            //LOG_INFO("WALL COLLISION STOP, %d   %d\n",mDistLeftR_mm, mDistRightR_mm);
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

MM::CommandResult MM::CollisionAvoidanceCommand::getResult()
{
    if(myWrappedCommandP)
    {
        return myWrappedCommandP->getResult();
    }
    else
    {
        // FIXME: this is not ideal, but it is not needed currently to run this wrapper on its own
        // This can be fixed by adding an encoder integrator object, and log the traveled distance inside this class as well!
        return CommandResult();
    }
}