#pragma once

#include <cmath>
#include "Unit.h"
#include "Ratio.h"
#include "Scalar.h"

// Compute sqrt, including on ratio (1000^2 -> 1000)
// Will not compile if you try to compute sqrt of a ratio that would be irrational (e.g. 1000)
template <GeneralScalar U>
    requires(IsUnit<U> && UnitExpableRatio<U, std::ratio<1, 2>>) || (!IsUnit<U>)
inline ScalarExp<U, std::ratio<1, 2>> unit_sqrt(const U val)
{
    if constexpr (IsUnit<U>)
    {
        using ResType = ScalarExp<U, std::ratio<1, 2>>;
        using Type = typename ResType::type;
        using Ratio = typename ResType::ratio;

        return ResType{DivideByRatio<Ratio, Type>(std::sqrt(val.GetRealValue()))};
    }
    else
    {
        return std::sqrt(val);
    }
}

template <typename T>
concept HasSquareRoot = requires(T a) {
    { unit_sqrt(a) };
};

template <typename>
struct SquareRootType_
{
    using type = UniversalFalse;
};

template <HasSquareRoot T>
    requires GeneralScalar<T>
struct SquareRootType_<T>
{
    using type = ScalarExp<T, std::ratio<1, 2>>;
};

template <typename T>
using SquareRootType = typename SquareRootType_<T>::type;

// Maintain ratio while computing pow
template <IsRatio Exp, GeneralScalar U>
inline ScalarExp<U, Exp> unit_pow(const U val)
{
    if constexpr (IsUnit<U>)
    {
        using Type = typename U::type;
        using Ratio = typename U::ratio;

        Type actual_val = MultiplyByRatio<Ratio, Type>(val.GetValue());

        return DivideByRatio<Ratio, Type>(std::pow(actual_val, RatioAsDouble<Exp>()));
    }
    else
    {
        return std::pow(val, RatioAsDouble<Exp>());
    }
}