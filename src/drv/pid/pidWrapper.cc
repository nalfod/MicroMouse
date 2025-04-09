#include "pidWrapper.h"

MM::PidWrapper::PidWrapper(double kp, double ki, double kd):
myPidController(&error, &output, &target, kp, kd, ki, DIRECT)
{

}

void MM::PidWrapper::setTarget(double newTarget)
{
    target = newTarget;
}

void MM::PidWrapper::compute(double newError)
{
    error = newError;
    myPidController.Compute();
}

double MM::PidWrapper::getOuput()
{
    return output;
}