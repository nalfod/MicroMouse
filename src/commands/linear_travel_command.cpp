#include "linear_travel_command.h"
#include "Arduino.h" // for millis()
#include "utils/logging.h"

////////////////////
///////   LinearTravelCommand
////////////////////

MM::LinearTravelCommand::LinearTravelCommand(float dist_um, 
                                             float speed_um_per_ms, 
                                             float acc_um_per_ms2, 
                                             float dec_um_per_ms2, 
                                             int64_t const& encoderValue1R, 
                                             int64_t const& encoderValue2R,
                                             int16_t& leftMotorVoltageR_mV,
                                             int16_t& rightMotorVoltageR_mV,
                                             LocationController& locController,
                                             bool isDummy):
myTargetSpeedCalculator(dist_um, speed_um_per_ms, acc_um_per_ms2, dec_um_per_ms2),
myEncIntegrator1(encoderValue1R),
myEncIntegrator2(encoderValue2R),
mLeftMotorVoltageR_mV(leftMotorVoltageR_mV),
mRightMotorVoltageR_mV(rightMotorVoltageR_mV),
mLocController(locController),
mDummy(isDummy)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrl.init(1, AUTOMATIC, -1000, 1000); // QUESTION: should the min and the max value be bounded to the current voltage somehow??
}

MM::LinearTravelCommand::LinearTravelCommand(float dist_um, 
                                             float speed_um_per_ms, 
                                             float acc_um_per_ms2, 
                                             float dec_um_per_ms2, 
                                             int64_t const& encoderValue1R, 
                                             int64_t const& encoderValue2R,
                                             int16_t& leftMotorVoltageR_mV,
                                             int16_t& rightMotorVoltageR_mV,
                                             LocationController& locController,
                                             bool isDummy, 
                                             double Kp, double Ki, double Kd):
myTargetSpeedCalculator(dist_um, speed_um_per_ms, acc_um_per_ms2, dec_um_per_ms2),
myEncIntegrator1(encoderValue1R),
myEncIntegrator2(encoderValue2R),
mLeftMotorVoltageR_mV(leftMotorVoltageR_mV),
mRightMotorVoltageR_mV(rightMotorVoltageR_mV),
mLocController(locController),
mDummy(isDummy),
myMovementCtrl(Kp, Ki, Kd)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrl.init(1, AUTOMATIC, -1000, 1000); // QUESTION: should the min and the max value be bounded to the current voltage somehow??
}


void MM::LinearTravelCommand::execute()
{
    if( mFinished ) { return; }

    unsigned long now_ms = millis();
    if( !mStarted )
    {
        mStartTime_ms = now_ms;
        mStarted = true;
        myEncIntegrator1.startIntegration();
        myEncIntegrator2.startIntegration();
    }
    
    // determining times
    unsigned long oldElapsedTime_ms = mElapsedTime_ms;
    mElapsedTime_ms = now_ms - mStartTime_ms;
    unsigned long timeChange_ms = mElapsedTime_ms - oldElapsedTime_ms;

    if( mElapsedTime_ms >= mTotalTimeOfTravel_ms )
    {
        mLeftMotorVoltageR_mV = 0;
        mRightMotorVoltageR_mV = 0;
        mFinished = true;
    }
    else
    {
        // Calculating feed forward; theoretical values:
        float outputSpeed_um_per_ms = myTargetSpeedCalculator.calcCurrentTargetSpeed_UmPerMs( mElapsedTime_ms );
        mDesiredCurrentPosition_um += outputSpeed_um_per_ms * timeChange_ms;
        
        // Real values
        int32_t traveledDistance_um = ( myEncIntegrator1.getTraveledDistanceSinceLastInvoke_Um() + myEncIntegrator2.getTraveledDistanceSinceLastInvoke_Um() ) / 2;
        mRealCurrentPosition_um += traveledDistance_um;
        if ( timeChange_ms != 0 )
        {
            mCurrentSpeed_UmPerMs = traveledDistance_um / static_cast<int32_t>( timeChange_ms );
        }
        
        // PID controlling
        myMovementCtrl.setTarget( static_cast<double>( mDesiredCurrentPosition_um ) );
        myMovementCtrl.compute( static_cast<double>( mRealCurrentPosition_um) );

        // Determining output voltage
        int16_t outputVoltage = static_cast<int16_t>(calcVoltageFromSpeed_mV(outputSpeed_um_per_ms)) + static_cast<int16_t>( myMovementCtrl.getOuput() );
        mLeftMotorVoltageR_mV = outputVoltage;
        mRightMotorVoltageR_mV = outputVoltage;                 
    }
}

int16_t MM::LinearTravelCommand::calcVoltageFromSpeed_mV( float setSpeed_um_per_ms )
{
    return static_cast<int16_t>(CONSTS::K_SPEED_FF * setSpeed_um_per_ms + CONSTS::K_BIAS_FF);
}

void MM::LinearTravelCommand::print() const
{
    LOG_INFO("LIN_TRAV_CMD: TOT_T: %d ELAPS_T: %d DDIST: %d RDIST: %d PID_OUT: %d \n",
        mTotalTimeOfTravel_ms,
        mElapsedTime_ms, 
        static_cast<int> ( mDesiredCurrentPosition_um / 1000 ),
        static_cast<int> ( mRealCurrentPosition_um / 1000 ),
        static_cast<int> ( myMovementCtrl.getOuput() )
      );
}

void MM::LinearTravelCommand::finishCommand()
{
    LOG_INFO("FINISH COMMAND: %d   %d\n",mStarted, mDummy);
    if(mStarted && !mDummy)
    {
        mLocController.moveInDirection();
        mLocController.updateWalls();
    }
}
