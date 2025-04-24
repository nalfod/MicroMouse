#include "wall_centering_command.h"
#include "constants.h"
#include "utils/logging.h"

MM::WallCenteringCommand::WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
mIrFrontLeftR(ir_frontleft),
mIrFrontRightR(ir_frontright),
myCurrentOriR_deg(currentOriR),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV),
myCenteringPidForWalls(0.05, 0, 0), // TODO: Maybe the tuning should be more sophisticated!!
myCenteringPidForOrientation(2, 0.2, 0) // TODO: Maybe the tuning should be more sophisticated!!
{
    myCenteringPidForWalls.setTarget( 0.0 );
    myCenteringPidForWalls.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
    
    myCenteringPidForOrientation.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
}

void MM::WallCenteringCommand::execute()
{
    if( !mStarted )
    {
        myCenteringPidForOrientation.setTarget( myCurrentOriR_deg );
        mStarted = true;
    }

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
    if( myWrappedCommandP.get() != nullptr )
    {   
        return myWrappedCommandP->isFinished();
    }
    else
    {
        // without wrapped command runs forever
        return false;
    }
}

bool MM::WallCenteringCommand::isCenteringWithWallsPossible() const
{
    return ( mIrFrontLeftR > CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING && mIrFrontRightR > CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING );
}

void MM::WallCenteringCommand::executeWallCenteringControl()
{
    if( isCenteringWithWallsPossible() )
    {
        executeCenteringUsingWallDistance();
        myCenteringPidForOrientation.setTarget( myCurrentOriR_deg ); // setting target for later
    }
    else
    {
        executeCenteringUsingOrientation();
    }
}

void MM::WallCenteringCommand::executeCenteringUsingWallDistance()
{
    myCenteringPidForWalls.compute( static_cast<double>( mIrFrontLeftR - mIrFrontRightR ) );
    mLeftMotorVoltageR_mV  -= static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
    mRightMotorVoltageR_mV += static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
}

void MM::WallCenteringCommand::executeCenteringUsingOrientation()
{
    myCenteringPidForOrientation.compute( myCurrentOriR_deg );
    mLeftMotorVoltageR_mV  += static_cast<int16_t>( myCenteringPidForOrientation.getOuput() );
    mRightMotorVoltageR_mV -= static_cast<int16_t>( myCenteringPidForOrientation.getOuput() );
}

MM::MotionCommandIF* MM::WallCenteringCommand::getWrappedObjectP()
{
    return myWrappedCommandP.get();
}

int16_t MM::WallCenteringCommand::getPidOutput() const
{
    return static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
}

int32_t MM::WallCenteringCommand::getCurrentError() const
{
    return mIrFrontLeftR - mIrFrontRightR;
}

void MM::WallCenteringCommand::print() const
{
    LOG_INFO("WALL_CENT_CMD");
    if( isCenteringWithWallsPossible() )
    {
        LOG_INFO("LED MODE - output: %d error: %d FL: %d FR: %d ", 
            static_cast<int16_t>( myCenteringPidForWalls.getOuput() ), 
            ( mIrFrontLeftR - mIrFrontRightR ),
            ( mIrFrontLeftR ),
            ( mIrFrontRightR ) );
    }
    else
    {
        LOG_INFO("IMU MODE - output: %d target: %d input: %d current_ori: %d ", 
            static_cast<int16_t>( myCenteringPidForOrientation.getOuput() ), 
            static_cast<int16_t>( myCenteringPidForOrientation.getTarget() ),
            static_cast<int16_t>( myCenteringPidForOrientation.getInput() ),
            static_cast<int16_t>( myCurrentOriR_deg ) );
    }
    LOG_INFO("\t");

    if( myWrappedCommandP.get() != nullptr )
    {  
        myWrappedCommandP->print();
    }
}