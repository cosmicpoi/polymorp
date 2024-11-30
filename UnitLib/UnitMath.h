#pragma once

#include <cmath>
#include "Unit.h"
#include "Ratio.h"

/**
 * Helpers for checking if types have exponents and getting results of exponentiating types
 */

template <typename, typename>
struct RatioExpType_
{
    using type = UniversalFalse;
};

template <IsUnit T, IsRatio Exp>
    requires RatioCanExp<typename T::ratio, Exp>
struct RatioExpType_<T, Exp>
{
    using type = UnitExp<T, Exp>;
};

template <typename T, IsRatio Exp>
    requires std::is_arithmetic_v<T>
struct RatioExpType_<T, Exp>
{
    using type = T;
};

/**
 * @brief ExpType<T> is the return type of `unit_pow(T val)`.
 * Defaults to `UniversalFalse` if exponent is not well-defined.
 */
template <typename T, IsRatio Exp>
using RatioExpType = typename RatioExpType_<T, Exp>::type;

/**
 * @brief SquareRootType<T> is the return type of `unit_sqrt(T val)`.
 * Defaults to `UniversalFalse` if square root is not well-defined.
 */
template <typename T>
using SquareRootType = RatioExpType<T, std::ratio<1, 2>>;

/**
 * Square root
 */

/** @brief HasSquareRoot - tells us if `unit_sqrt` and `SquareRootType` are well-defined for a type */
template <typename T>
concept HasSquareRoot = (IsUnit<T> && RatioCanExp<typename T::ratio, std::ratio<1, 2>>) || //
                        std::is_arithmetic_v<T>;

/**
 * @brief Generalized square root function for scalars (Units or plain types) .
 * If it's a unit, first compute sqrt of the ratio. So, sqrt(km^2) -> km, i.e. sqrt(10^6) -> 10^3
 * Otherwise, compute sqrt normally.
 * Will not compile if you try to compute sqrt of a ratio that would be irrational (e.g. 1000)
 * Only supports Units and `is_arithmetic` by default.
 */
template <typename Type, UnitIdentifier UID, IsRatio Ratio>
    requires RatioCanExp<Ratio, std::ratio<1, 2>>
inline UnitExp<Unit<Type, UID, Ratio>, std::ratio<1, 2>> unit_sqrt(const Unit<Type, UID, Ratio> &val)
{
    using ResRatio = typename UnitExp<Unit<Type, UID, Ratio>, std::ratio<1, 2>>::ratio;
    return UnitExp<Unit<Type, UID, Ratio>, std::ratio<1, 2>>{DivideByRatio<ResRatio, Type>(std::sqrt(val.GetRealValue()))};
}

template <typename T>
    requires std::is_arithmetic_v<T>
inline T unit_sqrt(const T &val)
{
    return std::sqrt(val);
}

/**
 * Rational power
 */

// TODO implement HasPow/HasExp and PowType/ExpType

/**
 * @brief Compute `val` to the power of rational exponent `Exp`.
 */
// template <IsRatio Exp, GeneralScalar U>
// inline ScalarExp<U, Exp> unit_pow(const U &val)
// {
//     if constexpr (IsUnit<U>)
//     {
//         using Type = typename U::type;
//         using Ratio = typename U::ratio;

//         Type actual_val = MultiplyByRatio<Ratio, Type>(val.GetValue());

//         return DivideByRatio<Ratio, Type>(std::pow(actual_val, RatioAsDouble<Exp>()));
//     }
//     else
//     {
//         return std::pow(val, RatioAsDouble<Exp>());
//     }
// }

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