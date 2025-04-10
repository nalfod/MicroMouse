#include "linear_travel_command.h"
#include "Arduino.h" // for millis()

////////////////////
///////   LinearTravelCommand
////////////////////

MM::LinearTravelCommand::LinearTravelCommand(float dist, 
                                             float speed, 
                                             float acc, 
                                             float dec, 
                                             int const& encoderValue1, 
                                             int const& encoderValue2,
                                             int16_t& leftMotorVoltage,
                                             int16_t& rightMotorVoltage):
myTargetSpeedCalculator(dist, speed, acc, dec),
myEncIntegrator1(encoderValue1),
myEncIntegrator2(encoderValue2),
mLeftMotorVoltage(leftMotorVoltage),
mRightMotorVoltage(rightMotorVoltage)
{
    mTotalTimeOfTravel = myTargetSpeedCalculator.getTotalTimeOfTravel();
    myMovementCtrl.init(1, AUTOMATIC, -1000, 1000);
}


void MM::LinearTravelCommand::execute()
{
    if( !mStarted )
    {
        mStartTime = millis();
        mStarted = true;
    }
    
    // determining times
    float now = millis();
    float timeChange = now - mElapsedTime;
    mElapsedTime = now - mStartTime;

    if( mElapsedTime >= mTotalTimeOfTravel )
    {
        mFinished = true;
    }
    else
    {
        float outputSpeed = myTargetSpeedCalculator.calcCurrentTargetSpeed( mElapsedTime );
        
        mRealCurrentPosition += ( myEncIntegrator1.getTraveledDistanceSinceLastInvoke() + myEncIntegrator2.getTraveledDistanceSinceLastInvoke() ) / 2;
        mDesiredCurrentPosition += outputSpeed * timeChange; 

        myMovementCtrl.setTarget( static_cast<double>( mDesiredCurrentPosition ) );
        myMovementCtrl.compute( static_cast<double>( mRealCurrentPosition) );

        int16_t outputVoltage = static_cast<int16_t>(calcVoltageFromSpeed(outputSpeed)) + static_cast<int16_t>( myMovementCtrl.getOuput() );

        mLeftMotorVoltage = outputVoltage;
        mRightMotorVoltage = mRightMotorVoltage;

        /* std::cout<<"Elapsed time= " << mElapsedTime << " Current speed= " << outputSpeed 
                 << " mDesiredCurrentPosition= " << mDesiredCurrentPosition << std::endl;*/ 
    }
}

float MM::LinearTravelCommand::calcVoltageFromSpeed( float setSpeed )
{
    return K_SPEED_FF * setSpeed + K_BIAS_FF;
}

//////////////////////////////////////
////////////// TargetSpeedCalculator
//////////////////////////////////////

MM::TargetSpeedCalculator::TargetSpeedCalculator(float dist, 
                                               float speed, 
                                               float acc, 
                                               float dec):
mDistance(dist),
mSetSpeed(speed),
mAcceleration(acc),
mDeceleration(dec)
{
    while( true )
    {
        mAccelerationTime = mSetSpeed / mAcceleration;
        mDecelerationTime = mSetSpeed / mDeceleration;
        mUniformTravelTime  = (mDistance - 0.5 * mSetSpeed * mSetSpeed * ( 1 / mAcceleration + 1 / mDeceleration ) ) / (mSetSpeed);

        if( mUniformTravelTime > 0.0 )
        {
            break;
        }
        else
        {
            mSetSpeed *= 0.9;
        }
    }
}

float MM::TargetSpeedCalculator::calcCurrentTargetSpeed(float elapsedTime)
{
    float targetSpeed = 0.0;
    
    if( elapsedTime < mAccelerationTime )
    {
        targetSpeed = calcVelocityInAcc( elapsedTime );
    }
    else if( elapsedTime > mAccelerationTime && elapsedTime < ( mAccelerationTime + mUniformTravelTime) )
    {
        targetSpeed = mSetSpeed;
    }
    else if( elapsedTime < (mAccelerationTime + mUniformTravelTime + mDecelerationTime) )
    {
        targetSpeed = calcVelocityInDec( elapsedTime - (mAccelerationTime + mUniformTravelTime) );
    }
    else
    {
        targetSpeed = 0;
    }
    
    return targetSpeed;
}

float MM::TargetSpeedCalculator::calcVelocityInAcc(float accElapsedTime)
{
    return accElapsedTime * mAcceleration;
}

float MM::TargetSpeedCalculator::calcVelocityInDec(float decElapsedTime)
{
    return mSetSpeed - (decElapsedTime * mDeceleration);
}