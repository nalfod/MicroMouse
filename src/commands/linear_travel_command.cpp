#include "linear_travel_command.h"
#include "Arduino.h" // for millis()
#include "utils/logging.h"

////////////////////
///////   LinearTravelCommand
////////////////////

MM::LinearTravelCommand::LinearTravelCommand(float dist_mm, 
                                             float speed_mm_per_s, 
                                             float acc_mm_per_s2, 
                                             float dec_mm_per_s2, 
                                             int64_t const& encoderValue1R, 
                                             int64_t const& encoderValue2R,
                                             int16_t& leftMotorVoltageR_mV,
                                             int16_t& rightMotorVoltageR_mV):
myTargetSpeedCalculator(std::abs(dist_mm), speed_mm_per_s, acc_mm_per_s2, dec_mm_per_s2),
myEncIntegrator1(encoderValue1R),
myEncIntegrator2(encoderValue2R),
mLeftMotorVoltageR_mV(leftMotorVoltageR_mV),
mRightMotorVoltageR_mV(rightMotorVoltageR_mV),
direction( dist_mm >= 0.0 ? 1 : -1)
{
    mTotalTimeOfTravel_ms = myTargetSpeedCalculator.getTotalTimeOfTravel_Ms();
    myMovementCtrl.init(1, AUTOMATIC, -1000, 1000); // QUESTION: should the min and the max value be bounded to the current voltage somehow??
}

MM::LinearTravelCommand::LinearTravelCommand(float dist_mm, 
                                             float speed_mm_per_s, 
                                             float acc_mm_per_s2, 
                                             float dec_mm_per_s2, 
                                             int64_t const& encoderValue1R, 
                                             int64_t const& encoderValue2R,
                                             int16_t& leftMotorVoltageR_mV,
                                             int16_t& rightMotorVoltageR_mV, 
                                             double Kp, double Ki, double Kd):
myTargetSpeedCalculator(std::abs(dist_mm), speed_mm_per_s, acc_mm_per_s2, dec_mm_per_s2),
myEncIntegrator1(encoderValue1R),
myEncIntegrator2(encoderValue2R),
mLeftMotorVoltageR_mV(leftMotorVoltageR_mV),
mRightMotorVoltageR_mV(rightMotorVoltageR_mV),
myMovementCtrl(Kp, Ki, Kd),
direction( dist_mm >= 0.0 ? 1 : -1)
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
        float outputSpeed_mm_per_s = myTargetSpeedCalculator.calcCurrentTargetSpeed_mmPerS( mElapsedTime_ms );
        mDesiredCurrentPosition_mm += static_cast<float>( outputSpeed_mm_per_s * timeChange_ms ) / 1000;
        
        // Real values
        float traveledDistance_mm = ( myEncIntegrator1.getTraveledDistanceSinceLastInvoke_mm() + myEncIntegrator2.getTraveledDistanceSinceLastInvoke_mm() ) / 2;
        mRealCurrentPosition_mm += direction * traveledDistance_mm;
        
        // PID controlling
        myMovementCtrl.setTarget( static_cast<double>( mDesiredCurrentPosition_mm ) );
        myMovementCtrl.compute( static_cast<double>( mRealCurrentPosition_mm) );

        // Determining output voltage
        int16_t outputVoltage = static_cast<int16_t>( calcVoltageFromSpeed_mV( outputSpeed_mm_per_s + static_cast<float>( myMovementCtrl.getOuput() ) ) );
        mLeftMotorVoltageR_mV = direction * outputVoltage;
        mRightMotorVoltageR_mV = direction * outputVoltage;
    }
}

int16_t MM::LinearTravelCommand::calcVoltageFromSpeed_mV( float setSpeed_mm_per_s )
{
    int16_t outputVoltageAbs_mV = static_cast<int16_t>(CONSTS::K_SPEED_FF * std::abs(setSpeed_mm_per_s) + CONSTS::K_BIAS_FF);

    if( setSpeed_mm_per_s > 0.0 )
    {
        return outputVoltageAbs_mV;
    }
    else if ( setSpeed_mm_per_s < 0.0 )
    {
        return -outputVoltageAbs_mV;
    }
    else
    {
        return 0;
    }
}

void MM::LinearTravelCommand::print() const
{
    LOG_INFO("LIN_TRAV_CMD: TOT_T: %d ELAPS_T: %d DDIST: %d RDIST: %d PID_OUT: %d \n",
        mTotalTimeOfTravel_ms,
        mElapsedTime_ms, 
        static_cast<int> ( mDesiredCurrentPosition_mm ),
        static_cast<int> ( mRealCurrentPosition_mm ),
        static_cast<int> ( myMovementCtrl.getOuput() )
      );
}

MM::CommandResult MM::LinearTravelCommand::getResult()
{
    if( direction > 0 )
    {
        return CommandResult(mRealCurrentPosition_mm, 0.0);
    }
    else
    {
        //FIXME: we dont log backward linear travel since it is towards a wall and it will be followed by a manual cell update
        //       but this is not ideal, because it reduce the available use-cases of this command
        return CommandResult(0, 0.0);
    }
}
