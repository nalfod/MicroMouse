#pragma once
#include <cstdint>
#include "constants.h"

namespace MM {

    class EncoderValueIntegrator
    {
    public:
        EncoderValueIntegrator(int64_t const& encoderResultVarR): currentEncoderValueR(encoderResultVarR) { }
    
        void startIntegration() { previousEncoderValue = currentEncoderValueR; } // this little error costed me ~4 hours of debugging :)
    
        float getTraveledDistanceSinceLastInvoke_mm()
        {
            float traveledDistance_mm = static_cast<float>( (currentEncoderValueR - previousEncoderValue) ) / static_cast<float>( CONSTS::ENCODER_RESOULTION ) * CONSTS::WHEEL_DIAMETER_MM * 3.1415;
            previousEncoderValue = currentEncoderValueR;
            return traveledDistance_mm;
        }
    
    private:
        int64_t const& currentEncoderValueR;
        int64_t previousEncoderValue;
    };

}