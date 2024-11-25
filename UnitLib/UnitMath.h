#pragma once

#include <cmath>
#include "Unit.h"
#include "Ratio.h"
#include "Scalar.h"

// Maintain ratio while computing sqrt
template <GeneralScalar U>
inline ScalarExp<U, std::ratio<1, 2>> unit_sqrt(const U val)
{
    if constexpr (IsUnit<U>)
    {
        using Type = typename U::type;
        using Ratio = typename U::ratio;

        Type actual_val = MultByRatio<Ratio>(val.GetValue());

        return DivideByRatio<Type, Ratio>(std::sqrt(actual_val));
    }
    else
    {
        return std::sqrt(val);
    }
}

// Maintain ratio while computing pow
template <IsRatio Exp, GeneralScalar U>
inline ScalarExp<U, Exp> unit_pow(const U val)
{
    if constexpr (IsUnit<U>)
    {
        using Type = typename U::type;
        using Ratio = typename U::ratio;

        Type actual_val = MultByRatio<Type, Ratio>(val.GetValue());

        return DivideByRatio<Type, Ratio>(std::pow(actual_val, RatioAsDouble<Exp>()));
    }
    else
    {
        return std::pow(val, RatioAsDouble<Exp>());
    }
}