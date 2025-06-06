#include "orientation_stabilizer.h"
#include "constants.h"
#include "utils/logging.h"

MM::OrientationStabilizer::OrientationStabilizer( float const& currentOriR, bool stayOnGrid ):
myCurrentOriR_deg( currentOriR ),
myPid(10, 0, 0),
stayOnGrid(stayOnGrid)
{
    myPid.setTarget( 0.0 );
    myPid.init( (static_cast<int>(CONSTS::MAIN_CYCLE_TIME.count()) / 1000) , AUTOMATIC, -750.0, 750.0); // TODO: Maybe the tuning should be more sofisticated!!
}

int16_t MM::OrientationStabilizer::executeControlling()
{
    float currentOriShifetd = _shiftOrientationValueRespectedToTarget( myCurrentOriR_deg );
    myPid.compute( currentOriShifetd );
    refreshIsLocked = true;
    return static_cast<int16_t>( myPid.getOuput() );
}

bool MM::OrientationStabilizer::isApplicable() const
{
    // always applicable!
    return true;
}

void MM::OrientationStabilizer::refreshMyTarget()
{
    if( !refreshIsLocked )
    {
        float targetOri_deg = myCurrentOriR_deg;
        if( stayOnGrid )
        {
            targetOri_deg = CONSTS::adjustAngleToAlignGridDirection( myCurrentOriR_deg );
        }
        myPid.setTarget( targetOri_deg );
    }

    refreshIsLocked = false;
}

void MM::OrientationStabilizer::print() const
{
    LOG_INFO("OrientationStabilizer - my target= %d\n", static_cast<int>( myPid.getTarget() ));
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