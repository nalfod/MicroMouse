#include "pidWrapper.h"

MM::PidWrapper::PidWrapper():
myPidController(&error, &output, &target, 0, 0, 0, DIRECT)
{

}

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
    error = newError; // RECONSIDER NAME!!!
    myPidController.Compute();
}

double MM::PidWrapper::getOuput() const
{
    return output; // MAYBE NO DOUBLE RETURN VALUE????? OR CAST IT????

}

void MM::PidWrapper::init(int sampleTime, int mode, double minOutputLimit, double maxOutputLimit)
{
    myPidController.SetSampleTime(sampleTime);
    myPidController.SetMode(mode);
    myPidController.SetOutputLimits(minOutputLimit, maxOutputLimit);
}

void MM::PidWrapper::setTunings(double Kp, double Ki, double Kd)
{
    myPidController.SetTunings(Kp, Ki, Kd);
}