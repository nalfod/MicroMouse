#include "wall_centering_command.h"
#include "constants.h"
#include "utils/logging.h"

MM::WallCenteringCommand::WallCenteringCommand(std::unique_ptr<MotionCommandIF> commandToWrap, uint16_t const& ir_frontleft, uint16_t const& ir_frontright, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
myWrappedCommandP(std::move(commandToWrap)),
mIrFrontLeftR(ir_frontleft),
mIrFrontRightR(ir_frontright),
mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
mRightMotorVoltageR_mV(rightMotorVoltage_mV),
myCenteringPidCtrl(0.05, 0, 0) // TODO: Maybe the tuning should be more sofisticated!!
{
    myCenteringPidCtrl.setTarget( 0.0 );
    myCenteringPidCtrl.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
}

void MM::WallCenteringCommand::execute()
{
    if( myWrappedCommandP.get() != nullptr )
    {
        myWrappedCommandP->execute();
        if( !( myWrappedCommandP->isFinished() ) )
        {
            myCenteringPidCtrl.compute( static_cast<double>( mIrFrontLeftR - mIrFrontRightR ) );
            mLeftMotorVoltageR_mV  -= static_cast<int16_t>( myCenteringPidCtrl.getOuput() );
            mRightMotorVoltageR_mV += static_cast<int16_t>( myCenteringPidCtrl.getOuput() );
        }
    }
    else
    {
        myCenteringPidCtrl.compute( static_cast<double>( mIrFrontLeftR - mIrFrontRightR ) );
        mLeftMotorVoltageR_mV  -= static_cast<int16_t>( myCenteringPidCtrl.getOuput() );
        mRightMotorVoltageR_mV += static_cast<int16_t>( myCenteringPidCtrl.getOuput() );
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

MM::MotionCommandIF* MM::WallCenteringCommand::getWrappedObjectP()
{
    return myWrappedCommandP.get();
}

int16_t MM::WallCenteringCommand::getPidOutput() const
{
    return static_cast<int16_t>( myCenteringPidCtrl.getOuput() );
}

int32_t MM::WallCenteringCommand::getCurrentError() const
{
    return mIrFrontLeftR - mIrFrontRightR;
}

void MM::WallCenteringCommand::print() const
{
    LOG_INFO("Wall center output: %d error: %d FL: %d FR: %d\n", static_cast<int16_t>( myCenteringPidCtrl.getOuput() ), 
    ( mIrFrontLeftR - mIrFrontRightR ),
    ( mIrFrontLeftR ),
    ( mIrFrontRightR ) );
    myWrappedCommandP->print();
}