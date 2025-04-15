#include "accerelometer.h"
#include "hal/micromouse.h"

volatile bool MM::Accelerometer::MPUInterrupt = false;

void MM::Accelerometer::DMPDataReady()
{
    MPUInterrupt = true;
}

void MM::Accelerometer::setupI2C()
{
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin(MM::PINS::IMU_SDA, MM::PINS::IMU_SCL);
        Wire.setClock(400000);
        delay(1000);
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
}

bool MM::Accelerometer::init()
{
    myMpu.initialize();
    if(myMpu.testConnection() ==  false){
        Serial.println("MPU6050 connection failed");
        return false;
    }
    else
    {
        Serial.println("MPU6050 connection successful");
        uint8_t devStatus = myMpu.dmpInitialize();
        myMpu.setRate(2); // QUESTION: how to properly adjust the clock rate with this???
        // myMpu.setDMPEnabled(true);
        myMpu.setXAccelOffset(0); //Set your accelerometer offset for axis X
        myMpu.setYAccelOffset(0); //Set your accelerometer offset for axis Y
        myMpu.setZAccelOffset(0); //Set your accelerometer offset for axis Z
        myMpu.setXGyroOffset(0);  //Set your gyro offset for axis X
        myMpu.setYGyroOffset(0);  //Set your gyro offset for axis Y
        myMpu.setZGyroOffset(0);  //Set your gyro offset for axis Z
        if(devStatus == 0)
        {
            myMpu.CalibrateAccel(6);  // Calibration Time: generate offsets and calibrate our MPU6050
            myMpu.CalibrateGyro(6);
            // Serial.println("These are the Active offsets: ");
            // myMpu.PrintActiveOffsets();
            myMpu.setDMPEnabled(true);

            /*Enable Arduino interrupt detection*/
            pinMode(MM::PINS::IMU_INT, INPUT);
            attachInterrupt(digitalPinToInterrupt(MM::PINS::IMU_INT), DMPDataReady, RISING);

            // This was in the example but i dont know now is it truly needed:
            // uint8_t  MPUIntStatus = mpu.getIntStatus();
            // uint16_t  packetSize = mpu.dmpGetFIFOPacketSize(); //Get expected DMP packet size for later comparison
            return true;
        }

        return false;
    }
}

bool MM::Accelerometer::loadSensorValues()
{
    uint8_t bufferReadResult = ( myMpu.dmpGetCurrentFIFOPacket(myFIFOBuffer) == 1 );
    myMpu.dmpGetQuaternion(&myQuaternionCont, myFIFOBuffer);

    // Determine yaw pitch roll
    myMpu.dmpGetGravity(&myGravityVec, &myQuaternionCont);
    myMpu.dmpGetYawPitchRoll(yawPithRoll_rad, &myQuaternionCont, &myGravityVec);

    // determine acceleration values (TODO: is it needed?)
    myMpu.dmpGetAccel(&myAccelSensorMeasurmentVec, myFIFOBuffer);
    myMpu.dmpGetLinearAccel(&myGravFreeAccelSensorMeasurmentVec, &myAccelSensorMeasurmentVec, &myGravityVec);
    return (bufferReadResult == 1);
}

void MM::Accelerometer::serialPrint()
{
    Serial.print("yawPithRoll [deg]:\t");
    Serial.print(yawPithRoll_rad[0] * 180/M_PI);
    Serial.print("\t");
    Serial.print(yawPithRoll_rad[1] * 180/M_PI);
    Serial.print("\t");
    Serial.println(yawPithRoll_rad[2] * 180/M_PI);
/*
    Serial.print("Real world acceleration [m/s2]:\t");
    Serial.print(myGravFreeAccelSensorMeasurmentVec.x);
    Serial.print("\t");
    Serial.print(myGravFreeAccelSensorMeasurmentVec.y);
    Serial.print("\t");
    Serial.println(myGravFreeAccelSensorMeasurmentVec.z);
    */
}