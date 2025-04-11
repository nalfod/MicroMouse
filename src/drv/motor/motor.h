#pragma once
#include <FS_MX1508.h>
#include <ESP32Encoder.h>

namespace MM {

class Motor
{
    public:
        enum RotationDirection
        {
            CLOCKWISE = 1,
            COUNTER_CLOCKWISE = 2
        };

        Motor(RotationDirection rotationDirection, int motorPin1, int motorPpin2, int encoderPin1, int encoderPin2);
    
        void setVoltageAndRotation(int16_t targetVoltage, 
                                   uint16_t currentBatteryVoltage);

        int64_t getEncoderCount();

    private:
            RotationDirection myRotationDirection;
            MX1508 myMotor;
            ESP32Encoder myEncoder;
};

} // namespace MM