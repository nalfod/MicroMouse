#pragma once

#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"

namespace MM {

class Accelerometer
{
    public:
        /*------Interrupt detection routine------*/
        static volatile bool MPUInterrupt;     // Indicates whether MPU6050 interrupt pin has gone high
        static void DMPDataReady();

        static void setupI2C();
        bool init();
        void loadSensorValues();

        // Output values
        int16_t mAx{0};
        int16_t mAy{0};
        int16_t mAz{0};
        int16_t mGx{0};
        int16_t mGy{0};
        int16_t mGz{0};
    private:
        MPU6050 myMpu{};
        uint8_t myFIFOBuffer[64]; // FIFO storage buffer
        /*---Orientation/Motion Variables---*/ 
        Quaternion q;           // [w, x, y, z]         Quaternion container
        VectorInt16 aa;         // [x, y, z]            Accel sensor measurements
        VectorInt16 gy;         // [x, y, z]            Gyro sensor measurements
        VectorInt16 aaReal;     // [x, y, z]            Gravity-free accel sensor measurements
        VectorInt16 aaWorld;    // [x, y, z]            World-frame accel sensor measurements
        VectorFloat gravity;    // [x, y, z]            Gravity vector
        float euler[3];         // [psi, theta, phi]    Euler angle container
        float ypr[3];           // [yaw, pitch, roll]   Yaw/Pitch/Roll container and gravity vector


        
};

} // namespace MM