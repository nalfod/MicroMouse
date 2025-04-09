#pragma once
#include <FS_MX1508.h>

namespace MM {

class Motor
{
    public:
        enum RotationDirection
        {
            CLOCKWISE = 1,
            COUNTER_CLOCKWISE = 2
        };

        Motor(int motorPin1, int motorPpin2): myMotor(motorPin1, motorPpin2) { }
    
        void setVoltageAndRotation(uint16_t targetVoltage, 
                                   RotationDirection rotationDirection, 
                                   uint16_t currentBatterVoltage);
        private:
            MX1508 myMotor;
};

} // namespace MM