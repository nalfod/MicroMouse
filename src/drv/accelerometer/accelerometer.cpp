#include "accerelometer.h"
#include "hal/micromouse.h"

void MM::Accelerometer::setupI2C()
{
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin(MM::PINS::IMU_SDA, MM::PINS::IMU_SCL);
        delay(2000);
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
}

bool MM::Accelerometer::init()
{
    myMpu.initialize();
    if(myMpu.testConnection() ==  false){
        return false;
    }
    else
    {
        // myMpu.dmpInitialize();
        // myMpu.setDMPEnabled(true);
        myMpu.setXAccelOffset(0); //Set your accelerometer offset for axis X
        myMpu.setYAccelOffset(0); //Set your accelerometer offset for axis Y
        myMpu.setZAccelOffset(0); //Set your accelerometer offset for axis Z
        myMpu.setXGyroOffset(0);  //Set your gyro offset for axis X
        myMpu.setYGyroOffset(0);  //Set your gyro offset for axis Y
        myMpu.setZGyroOffset(0);  //Set your gyro offset for axis Z
        return true;
    }
}

void MM::Accelerometer::loadSensorValues()
{
    myMpu.getMotion6(&mAx, &mAy, &mAz, &mGx, &mGy, &mGz);
    // myMpu.getAcceleration(&mAx, &mAy, &mAz);
    // myMpu.getRotation(&mGx, &mGy, &mGz);
    Serial.print("a/g:\t");
    Serial.print(mAx); Serial.print("\t");
    Serial.print(mAy); Serial.print("\t");
    Serial.print(mAz); Serial.print("\t");
    Serial.print(mGx); Serial.print("\t");
    Serial.print(mGy); Serial.print("\t");
    Serial.println(mGz);
}