#include "stuck_avoidance_command.h"

MM::StuckAvoidanceCommand::StuckAvoidanceCommand(std::unique_ptr<MotionCommandIF> commandToWrap, int64_t const& encoderValue1, int64_t const& encoderValue2, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
myEncIntegrator1(encoderValue1),
myEncIntegrator2(encoderValue2),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{}

void MM::StuckAvoidanceCommand::execute()
{
    if( myWrappedCommandP.get() != nullptr )
    {
        if( !mStarted )
        {
            mStarted = true;
            myEncIntegrator1.startIntegration();
            myEncIntegrator2.startIntegration();
        }

        if( myEncIntegrator1.getTraveledDistanceSinceLastInvoke_mm() < CONSTS::EPSILON && 
            myEncIntegrator2.getTraveledDistanceSinceLastInvoke_mm() < CONSTS::EPSILON )
        {
            stuck_counter++;
            if( stuck_counter >= 100 )
            {
                // If 5 ms the cycle time, it will kill the command if it is stucked for 0,5 sec
                mFinished = true;
                mLeftMotorVoltageR_mV = 0;
                mRightMotorVoltageR_mV = 0;
            }
        }
        else
        {
            stuck_counter = 0;
            myWrappedCommandP->execute();
        }
    }
    else
    {
        mFinished = true;
    }
}

bool MM::StuckAvoidanceCommand::isFinished() const
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

void MM::StuckAvoidanceCommand::print() const
{
    myWrappedCommandP->print();
}

MM::CommandResult MM::StuckAvoidanceCommand::getResult()
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