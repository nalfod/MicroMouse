#include "orientation_stabilizer.h"
#include "constants.h"
#include "utils/logging.h"

MM::OrientationStabilizer::OrientationStabilizer( float const& currentOriR ):
myCurrentOriR_deg( currentOriR ),
myPid(10, 0, 0)
{
    myPid.setTarget( 0.0 );
    myPid.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
}

int16_t MM::OrientationStabilizer::executeControlling()
{
    float currentOriShifetd = _shiftOrientationValueRespectedToTarget( myCurrentOriR_deg );
    myPid.compute( currentOriShifetd );
    return static_cast<int16_t>( myPid.getOuput() );
}

bool MM::OrientationStabilizer::isApplicable() const
{
    // always applicable!
    return true;
}

void MM::OrientationStabilizer::refreshMyTarget()
{
    myPid.setTarget( _adjustAngleToExactDirection( myCurrentOriR_deg ) );
}

void MM::OrientationStabilizer::print() const
{
    LOG_INFO("OrientationStabilizer applicable?= %d\n", static_cast<int>(isApplicable()));
}

// for cases, when target orientation is close to +180° or -180°, overflow prevention
double MM::OrientationStabilizer::_shiftOrientationValueRespectedToTarget(float currentOrientation)
{
    double retVal = static_cast<double>( currentOrientation );
    double target = myPid.getTarget();
    if( target - retVal < -180.0 )
    {
        retVal -= 360.0;
    }
    else if( target - retVal > 180.0 )
    {
        retVal += 360.0;
    }
    return retVal;
}

float MM::OrientationStabilizer::_adjustAngleToExactDirection( float currentOrientation )
{
    if( -30.0 < currentOrientation && currentOrientation < 30 )
    {
        currentOrientation = 0.0;
    }
    else if( 60.0 < currentOrientation && currentOrientation < 120.0 )
    {
        currentOrientation = 90.0;
    }
    else if( -120.0 < currentOrientation && currentOrientation < -60.0 )
    {
        currentOrientation = -90.0;
    }
    else if( -180.0 < currentOrientation && currentOrientation < -150.0 )
    {
        currentOrientation = -179.9;
    }
    else if( 150.0 < currentOrientation && currentOrientation < 180.0 )
    {
        currentOrientation = 179.9;
    }

    return currentOrientation;
}