#pragma once
#include "command_result.h"

namespace MM {

class MotionCommandIF
{
public:
    virtual ~MotionCommandIF() = default;
    virtual void execute() = 0;
    virtual CommandResult getResult() = 0;

    virtual bool isFinished() const = 0;
    virtual void print() const = 0;
};

} // namespace MM