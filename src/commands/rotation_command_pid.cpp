#include "rotation_command_pid.h"
 #include <cstdlib>
 #include "utils/logging.h"
 #include "constants.h"
 
 MM::RotationCommandPid::RotationCommandPid(RotationOrientation direction, float angleToRotate_deg, float const& currentOriR, int16_t& leftMotorVoltage_mV, int16_t& rightMotorVoltage_mV):
 myDircetion(direction),
 myTargetMagnitude_deg(angleToRotate_deg),
 myCurrentOriR_deg(currentOriR),
 mLeftMotorVoltageR_mV(leftMotorVoltage_mV),
 mRightMotorVoltageR_mV(rightMotorVoltage_mV)
 {
     myMovementCtrl.init(1, AUTOMATIC, CONSTS::K_BIAS_FF * 1.2 , 1400); // QUESTION: should the min and the max value be bounded to the current voltage somehow??
     myMovementCtrl.setTarget( static_cast<double>( myTargetMagnitude_deg ) );
 }
 
 void MM::RotationCommandPid::execute()
 {
     if( mFinished ) { return; }
 
     if( !mStarted )
     {
         myPreviousOrientation_deg = myCurrentOriR_deg;
         mStarted = true;
     }
 
     if( myTraveledDistance_deg >= myTargetMagnitude_deg)
     {
         // TODO: maybe the final motor voltage adjustment should be the responsibility of the caller
         mLeftMotorVoltageR_mV = 0;
         mRightMotorVoltageR_mV = 0;
         mFinished = true;
     }
     else
     {
         myTraveledDistance_deg += std::abs( std::abs( myCurrentOriR_deg ) - std::abs( myPreviousOrientation_deg ) );
         myPreviousOrientation_deg = myCurrentOriR_deg;
 
         myMovementCtrl.compute( static_cast<double>( myTraveledDistance_deg ) );
         int16_t voltage_mV = static_cast<int16_t>( myMovementCtrl.getOuput() );
 
         if( myDircetion == CLOCKWISE )
         {
             mLeftMotorVoltageR_mV = voltage_mV;
             mRightMotorVoltageR_mV = -voltage_mV;
         }
         else
         {
             mLeftMotorVoltageR_mV = -voltage_mV;
             mRightMotorVoltageR_mV = +voltage_mV;
         }
     }
 }
 
 void MM::RotationCommandPid::print() const
 {
     LOG_INFO("GOAL: %d TRAVELD: %d PID_OUT: %d ",
         static_cast<int> ( myTargetMagnitude_deg ),
         static_cast<int> ( myTraveledDistance_deg ), 
         static_cast<int> ( myMovementCtrl.getOuput() )
       );
 }