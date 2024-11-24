#pragma once

#include <cmath>
#include "Unit.h"

// Get type for exponentiated unit
template <IsUnit U, IsRatio Exp>
using ExpUnit_T = Unit<
    typename U::type,
    UIExp<typename U::uid, Exp>,
    typename U::ratio>;

// Maintain ratio while computing sqrt
template <IsUnit U>
inline ExpUnit_T<U, std::ratio<1, 2>> unit_sqrt(U val)
{
    using Type = typename U::type;
    using Ratio = typename U::ratio;

    Type actual_val = MultRatio<Type, Ratio>(val.value);

    return DivRatio<Type, Ratio>(std::sqrt(actual_val));
}

// Maintain ratio while computing pow
template <IsRatio Exp, IsUnit U>
inline ExpUnit_T<U, Exp> unit_pow(U val)
{
    using Type = typename U::type;
    using Ratio = typename U::ratio;

    Type actual_val = MultRatio<Type, Ratio>(val.value);

    return DivRatio<Type, Ratio>(std::pow(actual_val, RatioAsDouble<Exp>()));
}