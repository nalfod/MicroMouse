#include "two_wall_stabilizer.h"
#include "constants.h"
#include "utils/logging.h"

MM::TwoWallStabilizer::TwoWallStabilizer( uint16_t const& dist_frontleft, uint16_t const& dist_frontright ):
mDistFrontLeftR_mm(dist_frontleft),
mDistFrontRightR_mm(dist_frontright),
myPid(8, 0, 0)
{
    myPid.setTarget( 0.0 );
    myPid.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
}

int16_t MM::TwoWallStabilizer::executeControlling()
{
    myPid.compute( static_cast<double>( mDistFrontLeftR_mm - mDistFrontRightR_mm ) );
    return static_cast<int16_t>( myPid.getOuput() );
}

bool MM::TwoWallStabilizer::isApplicable() const
{
    return ( mDistFrontLeftR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM && mDistFrontRightR_mm < CONSTS::WALL_DISTANCE_LIMIT_FOR_CENTERING_MM );
}

void MM::TwoWallStabilizer::print() const
{
    LOG_INFO("TwoWallStabilizer applicable?= %d\n", static_cast<int>(isApplicable()));
}