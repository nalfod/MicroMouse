#pragma once

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps612.h"

namespace MM {

class AngularRotationComputer
{
    public:
        AngularRotationComputer(float const& yawValueR): currentYawValueR(yawValueR) 
        { 
            isValuePositive = ( currentYawValueR > 0);
        }
        void startMeasurement() { startTime = millis(); }

        void refresh()
        {
            if( (isValuePositive && currentYawValueR < 0) || 
                (!isValuePositive && currentYawValueR > 0) )
            {
                isValuePositive = !isValuePositive;
                traveledHalfCircles++;
            }
        }

        uint32_t getAngularVelocity_miliRevPerS()
        {
            uint32_t elapsedTimeMs = static_cast<uint32_t>( millis() - startTime );
            return (traveledHalfCircles * 500000) / elapsedTimeMs ;
        }
    
        float const& currentYawValueR;
        bool isValuePositive;
        uint32_t traveledHalfCircles{0};
        unsigned long startTime{0};

};

class Accelerometer
{
    public:
        /*------Interrupt detection routine------*/
        static volatile bool MPUInterrupt;     // Indicates whether MPU6050 interrupt pin has gone high
        static void DMPDataReady();

        /*------Init functions------*/
        static void setupI2C();
        bool init();
        bool dmp_set_fifo_divider(uint16_t divider);

        /*------FIFO loader and getters------*/
        bool loadSensorValues();
        float getCurrentYawValue();

        // for debug
        void serialPrint();
        AngularRotationComputer myAngRotMeter{yawPithRoll_rad[0]};

    private:
        MPU6050 myMpu{};
        uint8_t myFIFOBuffer[64]; // FIFO storage buffer
        /*---Orientation/Motion Variables---*/ 
        Quaternion myQuaternionCont;           // [w, x, y, z]         Quaternion container
        //VectorInt16 myAccelSensorMeasurmentVec;         // [x, y, z]            Accel sensor measurements
        //VectorInt16 gy;         // [x, y, z]            Gyro sensor measurements
        //VectorInt16 myGravFreeAccelSensorMeasurmentVec;     // [x, y, z]            Gravity-free accel sensor measurements
        //VectorInt16 aaWorld;    // [x, y, z]            World-frame accel sensor measurements
        VectorFloat myGravityVec;    // [x, y, z]            Gravity vector
        //float euler[3];         // [psi, theta, phi]    Euler angle container
        float yawPithRoll_rad[3];           // [yaw, pitch, roll]   Yaw/Pitch/Roll container and gravity vector
        
};

} // namespace MM