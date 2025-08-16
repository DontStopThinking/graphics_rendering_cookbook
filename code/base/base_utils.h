#pragma once
#include "base/base_types.h"

inline u64 AlignToNextPow2(u64 value, u64 multiple)
{
    if (multiple == 0)
    {
        return value;
    }
    return (value + multiple - 1) & ~(multiple - 1);
}
