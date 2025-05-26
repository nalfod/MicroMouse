#include "wall_centering_command.h"
#include "constants.h"
#include "utils/logging.h"

MM::WallCenteringCommand::WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, std::vector<std::unique_ptr<MM::MovementStabilizerIF>>&& stabilizers, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
myStabilizers(std::move(stabilizers)),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV)
{

}

void MM::WallCenteringCommand::execute()
{
    if( myWrappedCommandP.get() != nullptr )
    {
        myWrappedCommandP->execute();
        if( !( myWrappedCommandP->isFinished() ) )
        {
            executeWallCenteringControl();
        }
    }
    else
    {
        // without wrapped command runs forever
        executeWallCenteringControl();
    }
}

bool MM::WallCenteringCommand::isFinished() const
{
    //print();
    if( myWrappedCommandP.get() != nullptr )
    {   
        return myWrappedCommandP->isFinished();
    }
    else
    {
        // without wrapped command, runs forever
        return false;
    }
}

void MM::WallCenteringCommand::executeWallCenteringControl()
{
    bool stabilizationDone = false;
    for (const auto& stabilizer : myStabilizers) {
        stabilizer->refreshMyTarget(); // honestly only needed for the orientation strategy so might be omitted if encoder based strategy is introduced
        if (stabilizer->isApplicable() && !stabilizationDone) {
            //stabilizer->print();
            int16_t output = stabilizer->executeControlling();
            mLeftMotorVoltageR_mV  += output;
            mRightMotorVoltageR_mV -= output;
            stabilizationDone = true;
        }
    }
}

// FOR DEBUG
void MM::WallCenteringCommand::print() const
{
    /*
    LOG_INFO("WALL_CENT_CMD   ");
    if( isCenteringWithWallsPossible() )
    {
        LOG_INFO("LED MODE - output: %d error: %d FL: %d FR: %d ", 
            static_cast<int16_t>( myCenteringPidForWalls.getOuput() ), 
            ( mDistFrontLeftR_mm - mDistFrontRightR_mm ),
            ( mDistFrontLeftR_mm ),
            ( mDistFrontRightR_mm ) );
    }
    else
    {
        LOG_INFO("IMU MODE - output: %d target: %d input: %d current_ori: %d ", 
            static_cast<int16_t>( myCenteringPidForOrientation.getOuput() ), 
            static_cast<int16_t>( myCenteringPidForOrientation.getTarget() ),
            static_cast<int16_t>( myCenteringPidForOrientation.getInput() ),
            static_cast<int16_t>( myCurrentOriR_deg ) );
    }
    LOG_INFO("\n");*/

    if( myWrappedCommandP.get() != nullptr )
    {  
        myWrappedCommandP->print();
    }
}

MM::CommandResult MM::WallCenteringCommand::getResult()
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