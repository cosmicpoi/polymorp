#pragma once

#include <cmath>
#include "Unit.h"

// Maintain ratio while computing sqrt
template <IsUnit U>
inline UnitExp<U, std::ratio<1, 2>> unit_sqrt(U val)
{
    using Type = typename U::type;
    using Ratio = typename U::ratio;

    Type actual_val = RatioMult<Type, Ratio>(val.value);

    return RatioDiv<Type, Ratio>(std::sqrt(actual_val));
}

// Maintain ratio while computing pow
template <IsRatio Exp, IsUnit U>
inline UnitExp<U, Exp> unit_pow(U val)
{
    using Type = typename U::type;
    using Ratio = typename U::ratio;

    Type actual_val = RatioMult<Type, Ratio>(val.value);

    return RatioDiv<Type, Ratio>(std::pow(actual_val, RatioAsDouble<Exp>()));
}