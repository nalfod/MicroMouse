#pragma once
#include <PID_v1.h>

namespace MM {

class PidWrapper
{
    public:
        PidWrapper();
        PidWrapper(double kp, double ki, double kd);

        void init(int sampleTime, int mode, double minOutputLimit, double maxOutputLimit);

        void setTarget(double newTarget);
        void compute(double newError);
        double getOuput() const;
        double getInput() const;
        double getTarget() const;

        void setTunings(double Kp, double Ki, double Kd);

    private:
        double input{0.0};
        double output{0.0};
        double target{0.0};
        PID myPidController;
};

} // namespace MM