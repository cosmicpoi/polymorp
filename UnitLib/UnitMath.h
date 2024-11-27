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

        return DivideByRatio<Ratio, Type>(std::sqrt(actual_val));
    }
    else
    {
        return std::sqrt(val);
    }
}

template <typename T>
concept HasSquareRoot = requires(T a) {
    { unit_sqrt(a) };
    { unit_sqrt(a) * unit_sqrt(a) } -> std::same_as<T>;
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

        Type actual_val = MultByRatio<Ratio, Type>(val.GetValue());

        return DivideByRatio<Ratio, Type>(std::pow(actual_val, RatioAsDouble<Exp>()));
    }
    else
    {
        return std::pow(val, RatioAsDouble<Exp>());
    }
}