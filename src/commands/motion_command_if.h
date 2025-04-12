#pragma once

namespace MM {

class MotionCommandIF
{
public:
    virtual ~MotionCommandIF() = default;
    virtual void execute() = 0;
    virtual bool isFinished() = 0;
};

} // namespace MM