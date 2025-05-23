#pragma once
#include <cstdint>

namespace MM {

class MovementStabilizerIF
{
public:
    virtual ~MovementStabilizerIF() = default;
    virtual int16_t executeControlling() = 0;
    virtual bool isApplicable() const = 0;
    virtual void refreshMyTarget() = 0;

    virtual void print() const = 0;
};

} // namespace MM