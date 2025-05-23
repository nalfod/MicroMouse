#include "wall_centering_command.h"
#include "constants.h"
#include "utils/logging.h"

MM::WallCenteringCommand::WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& dist_frontleft, uint16_t const& dist_frontright, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
mDistFrontLeftR_mm(dist_frontleft),
mDistFrontRightR_mm(dist_frontright),
myCurrentOriR_deg(currentOriR),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV),
myCenteringPidForWalls(8, 0, 0), // TODO: Maybe the tuning should be more sophisticated!!
myCenteringPidForOrientation(10, 0, 0) // TODO: Maybe the tuning should be more sophisticated!!
{
    myCenteringPidForWalls.setTarget( 0.0 );
    myCenteringPidForWalls.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
    
    myCenteringPidForOrientation.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
}

void MM::WallCenteringCommand::execute()
{
    if( !mStarted )
    {
        myCenteringPidForOrientation.setTarget( adjustAngleToExactDirection( myCurrentOriR_deg ) );
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
    //print();
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
    return ( mDistFrontLeftR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM || mDistFrontRightR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM );
}

void MM::WallCenteringCommand::executeWallCenteringControl()
{
    if( isCenteringWithWallsPossible() )
    {
        executeCenteringUsingWallDistance();
        myCenteringPidForOrientation.setTarget( adjustAngleToExactDirection( myCurrentOriR_deg ) ); // saving target for a time when there is no two sidewalls
    }
    else
    {
        executeCenteringUsingOrientation();
    }
}

void MM::WallCenteringCommand::executeCenteringUsingWallDistance()
{
    if( !(mDistFrontLeftR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM && mDistFrontRightR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM))
    {
        if (mDistFrontLeftR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM)
        {
            myCenteringPidForWalls.compute( static_cast<double>(  CONSTS::WALL_DISTANCE_MID_FOR_CENTERING_MM - mDistFrontLeftR_mm ) );
            mLeftMotorVoltageR_mV  -= static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
            mRightMotorVoltageR_mV += static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
        }
        else
        {
            myCenteringPidForWalls.compute( static_cast<double>(  CONSTS::WALL_DISTANCE_MID_FOR_CENTERING_MM - mDistFrontRightR_mm ) );
            mLeftMotorVoltageR_mV  += static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
            mRightMotorVoltageR_mV -= static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
        }
    }
    else
    {
        myCenteringPidForWalls.compute( static_cast<double>( mDistFrontLeftR_mm - mDistFrontRightR_mm ) );
        mLeftMotorVoltageR_mV  += static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
        mRightMotorVoltageR_mV -= static_cast<int16_t>( myCenteringPidForWalls.getOuput() );
    }
}

void MM::WallCenteringCommand::executeCenteringUsingOrientation()
{
    float currentOriShifetd = shiftOrientationValueRespectedToTarget( myCurrentOriR_deg );
    myCenteringPidForOrientation.compute( currentOriShifetd );
    mLeftMotorVoltageR_mV  += static_cast<int16_t>( myCenteringPidForOrientation.getOuput() );
    mRightMotorVoltageR_mV -= static_cast<int16_t>( myCenteringPidForOrientation.getOuput() );
}

// for cases, when target orientation is close to +180° or -180°, overflow prevention
double MM::WallCenteringCommand::shiftOrientationValueRespectedToTarget(float currentOrientation)
{
    double retVal = static_cast<double>( currentOrientation );
    double target = myCenteringPidForOrientation.getTarget();
    if( target - retVal < -180.0 )
    {
        retVal -= 360.0;
    }
    else if( target - retVal > 180.0 )
    {
        retVal += 360.0;
    }
    return retVal;
}

float MM::WallCenteringCommand::adjustAngleToExactDirection( float currentOrientation )
{
    if( -30.0 < currentOrientation && currentOrientation < 30 )
    {
        currentOrientation = 0.0;
    }
    else if( 60.0 < currentOrientation && currentOrientation < 120.0 )
    {
        currentOrientation = 90.0;
    }
    else if( -120.0 < currentOrientation && currentOrientation < -60.0 )
    {
        currentOrientation = -90.0;
    }
    else if( -180.0 < currentOrientation && currentOrientation < -150.0 )
    {
        currentOrientation = -179.9;
    }
    else if( 150.0 < currentOrientation && currentOrientation < 180.0 )
    {
        currentOrientation = 179.9;
    }

    return currentOrientation;
}

// FOR DEBUG
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
    return mDistFrontLeftR_mm - mDistFrontRightR_mm;
}

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