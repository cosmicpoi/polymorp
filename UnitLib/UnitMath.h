#pragma once

#include <cmath>
#include "Unit.h"
#include "Ratio.h"

/**
 * Concepts for plain scalars and 
 */

template <typename T>
concept PlainScalar = !IsUnit<T> && std::is_arithmetic_v<T>;

template <typename T>
concept GeneralScalar = IsUnit<T> || PlainScalar<T>; //

/**
 * ScalarExp type helpers
 */


template <GeneralScalar T, IsRatio Exp>
struct ScalarExp_
{
};

/* ScalarExp helper */

template <IsUnit U, IsRatio Exp>
struct ScalarExp_<U, Exp>
{
    using type = UnitExp<U, Exp>;
};

template <PlainScalar T, IsRatio Exp>
struct ScalarExp_<T, Exp>
{
    using type = T;
};

template <GeneralScalar T, IsRatio Exp>
using ScalarExp = typename ScalarExp_<T, Exp>::type;

/**
 * Square root
 */

/** @brief HasSquareRoot - tells us if `unit_sqrt` and `SquareRootType` are well-defined for a type */
template <typename T>
concept HasSquareRoot = (IsUnit<T> && UnitExpableRatio<T, std::ratio<1, 2>>) || //
                        (PlainScalar<T>);

/**
 * @brief Generalized square root function for scalars (Units or plain types) .
 * If it's a unit, first compute sqrt of the ratio. So, sqrt(km^2) -> km, i.e. sqrt(10^6) -> 10^3
 * Otherwise, compute sqrt normally.
 * Will not compile if you try to compute sqrt of a ratio that would be irrational (e.g. 1000)
 */
template <GeneralScalar U>
    requires HasSquareRoot<U>
inline ScalarExp<U, std::ratio<1, 2>> unit_sqrt(const U &val)
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

// Helpers for `SquareRootType`

template <typename>
struct SquareRootType_
{
    using type = UniversalFalse;
};

template <HasSquareRoot T>
struct SquareRootType_<T>
{
    using type = ScalarExp<T, std::ratio<1, 2>>;
};

/**
 * @brief SquareRootType<T> is the return type of `unit_sqrt(T val)`.
 * Defaults to `UniversalFalse` if square root is not well-defined.
 */
template <typename T>
using SquareRootType = typename SquareRootType_<T>::type;

/**
 * Rational power
 */

// TODO implement HasPow/HasExp and PowType/ExpType

/**
 * @brief Compute `val` to the power of rational exponent `Exp`.
 */
template <IsRatio Exp, GeneralScalar U>
inline ScalarExp<U, Exp> unit_pow(const U &val)
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

/**
 * Absolute value
 */
template <typename Type>
    requires requires(Type a) {
        { a > Type{0} } -> std::convertible_to<bool>;
        { -a } -> std::constructible_from<Type>;
    }
inline Type unit_abs(const Type &val)
{
    return (val > Type{0}) ? (val) : -val;
}