#include "globals.h"

MM::Globals::Globals(): 
myStraightMovementCtrl(0.05, 0, 0) 
{
    // measured value is the diffrence between the two sides
    // max signal: next to the wall: 4000 the other: 400
    // equal: both around 1000
    myStraightMovementCtrl.setTarget(0);
}