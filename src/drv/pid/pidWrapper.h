#pragma once
#include <PID_v1.h>

namespace MM {

class PidWrapper
{
    public:
        PidWrapper(double kp, double ki, double kd);

        void setTarget(double newTarget);
        void compute(double newError);
        double getOuput();

    private:
        double error;
        double output;
        double target;
        PID myPidController;
};

} // namespace MM