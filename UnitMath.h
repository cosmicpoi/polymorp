#pragma once

#include <cmath>
#include "Unit.h"


template<IsUnit U>
inline HalfExp<U> squareRoot(U val)
{
    if constexpr (std::is_integral_v<typename U::type>)
    {
        return (typename U::type) std::sqrt(val.GetValue());
    }
    else
    {
        return std::sqrt(val.GetValue());
    }
}