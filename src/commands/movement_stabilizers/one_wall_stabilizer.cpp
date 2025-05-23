#include "one_wall_stabilizer.h"
#include "constants.h"
#include "utils/logging.h"

MM::OneWallStabilizer::OneWallStabilizer(uint16_t const& dist_frontleft, uint16_t const& dist_frontright):
TwoWallStabilizer(dist_frontleft, dist_frontright)
{

}

int16_t MM::OneWallStabilizer::executeControlling()
{
    // TODO: TEST THIS might not work. 
    if (mDistFrontLeftR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM)
    {
        myPid.compute( static_cast<double>(  mDistFrontLeftR_mm - CONSTS::WALL_DISTANCE_MID_FOR_CENTERING_MM ) );
    }
    else
    {
        myPid.compute( static_cast<double>(  CONSTS::WALL_DISTANCE_MID_FOR_CENTERING_MM - mDistFrontRightR_mm ) );
    }

    // mLeftMotorVoltageR_mV  += and mRightMotorVoltageR_mV -= shall be applied with the result!
    return static_cast<int16_t>( myPid.getOuput() );
}

bool MM::TwoWallStabilizer::isApplicable() const
{
    return ( mDistFrontLeftR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM || mDistFrontRightR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM );
}

void MM::TwoWallStabilizer::print() const
{
    LOG_INFO("OneWallStabilizer applicable?= %d\n", static_cast<int>(isApplicable()));
}