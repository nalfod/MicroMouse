#pragma once

#include "I2Cdev.h"
#include <MPU6050.h>

namespace MM {

class Accelerometer
{
    public:
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
        
};

} // namespace MM