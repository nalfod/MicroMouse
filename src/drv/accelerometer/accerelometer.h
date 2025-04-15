#pragma once

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

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

        uint32_t getAngularVelocity_revPerS()
        {
            return uint32_t(traveledHalfCircles / 2) / uint32_t( ( millis() - startTime ) ) * 1000;
        }
    
        float const& currentYawValueR;
        bool isValuePositive;
        int traveledHalfCircles{0};
        unsigned long startTime{0};

};

class Accelerometer
{
    public:
        /*------Interrupt detection routine------*/
        static volatile bool MPUInterrupt;     // Indicates whether MPU6050 interrupt pin has gone high
        static void DMPDataReady();

        static void setupI2C();
        bool init();
        bool loadSensorValues();

        // for debug
        void serialPrint();
        AngularRotationComputer myAngRotMeter{yawPithRoll_rad[0]};

    private:
        MPU6050 myMpu{};
        uint8_t myFIFOBuffer[64]; // FIFO storage buffer
        /*---Orientation/Motion Variables---*/ 
        Quaternion myQuaternionCont;           // [w, x, y, z]         Quaternion container
        VectorInt16 myAccelSensorMeasurmentVec;         // [x, y, z]            Accel sensor measurements
        //VectorInt16 gy;         // [x, y, z]            Gyro sensor measurements
        VectorInt16 myGravFreeAccelSensorMeasurmentVec;     // [x, y, z]            Gravity-free accel sensor measurements
        //VectorInt16 aaWorld;    // [x, y, z]            World-frame accel sensor measurements
        VectorFloat myGravityVec;    // [x, y, z]            Gravity vector
        //float euler[3];         // [psi, theta, phi]    Euler angle container
        float yawPithRoll_rad[3];           // [yaw, pitch, roll]   Yaw/Pitch/Roll container and gravity vector
        
};

} // namespace MM