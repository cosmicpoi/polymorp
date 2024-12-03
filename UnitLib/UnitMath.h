#pragma once

#include <cmath>
#include "Unit.h"
#include "Ratio.h"

/**
 * Generalized square root function for scalars (Units or plain types) .
 * If it's a unit, first compute sqrt of the ratio. So, sqrt(km^2) -> km, i.e. sqrt(10^6) -> 10^3
 * Otherwise, compute sqrt normally.
 * Will not compile if you try to compute sqrt of a ratio that would be irrational (e.g. 1000)
 * Only supports Units and `is_arithmetic` by default.
 */

/** @brief Square root for unit types. Will not compile if `sqrt(Unit_Ratio)` is not rational (e.g. for kilometers) */
template <typename Type, UnitIdentifier UID, IsRatio Ratio>
    requires requires(Type a) {
        { std::sqrt(a) };
    } && RatioCanExp<Ratio, std::ratio<1, 2>>
inline UnitExp<Unit<Type, UID, Ratio>, std::ratio<1, 2>> unit_sqrt(const Unit<Type, UID, Ratio> &val)
{
    using ResRatio = typename UnitExp<Unit<Type, UID, Ratio>, std::ratio<1, 2>>::ratio;
    return UnitExp<Unit<Type, UID, Ratio>, std::ratio<1, 2>>{
        DivideByRatio<ResRatio, Type>(std::sqrt(val.GetRealValue()))};
}

/** @brief Square root for IsArithmetic types */
template <typename T>
    requires requires(T a) {
        { std::sqrt(a) };
    }
inline T unit_sqrt(const T &val)
{
    return std::sqrt(val);
}

/** @brief Concept to check if `unit_sqrt` is defined for a type */
template <typename T>
concept HasSquareRoot = requires(T a) {
    { unit_sqrt(a) };
};

/** @brief SquareRootType<T> is the return type of `unit_sqrt(T val)`. */
template <typename T>
    requires HasSquareRoot<T>
using SquareRootType = decltype(unit_sqrt(std::declval<T>()));

/**
 * Generalized rational power function for scalars (Units or plain types) .
 * If it's a unit, first compute power of the ratio. So, sqrt(km^2/3) -> 10^2, (i.e. 10^6^2/3->10^2)
 * Otherwise, compute pow normally
 * Will not compile if you try to compute pow of a ratio that would be irrational (e.g. 1000^1/2)
 * Only supports Units and `is_arithmetic` by default.
 */

/** @brief Rational pow for unit types. Will not compile if `Unit_Ratio^(Exp)` is not rational (e.g. Km^1/2) */
template <IsRatio Exp, typename Type, UnitIdentifier UID, IsRatio Ratio>
    requires requires(Type a, double b) {
        { std::pow(a, b) };
    } && RatioCanExp<Ratio, Exp>
inline UnitExp<Unit<Type, UID, Ratio>, Exp> unit_ratio_pow(const Unit<Type, UID, Ratio> &val)
{
    using ResRatio = typename UnitExp<Unit<Type, UID, Ratio>, Exp>::ratio;
    return UnitExp<Unit<Type, UID, Ratio>, Exp>{
        DivideByRatio<ResRatio, Type>(std::pow(val.GetRealValue(), RatioAsDouble<Exp>()))};
}

/** @brief Rational pow for IsArithmetic types */
template <IsRatio Exp, typename T>
    requires requires(T a, double b) {
        { std::pow(a, b) };
    }
inline T unit_ratio_pow(const T &val)
{
    return std::pow(val, RatioAsDouble<Exp>());
}

/** @brief Concept to check if `unit_ratio_pow` is defined for a type */
template <typename Exp, typename T>
concept HasRatioPow = requires(T a) {
    requires IsRatio<Exp>;
    { unit_ratio_pow<Exp>(a) };
};

/** @brief RatioPowType<Exp, T> is the return type of `unit_ratio_pow<Exp>(T val)`. */
template <IsRatio Exp, typename T>
    requires HasRatioPow<Exp, T>
using RatioPowType = decltype(unit_ratio_pow<Exp>(std::declval<T>()));

/**
 * Absolute value
 */

/** @brief Concept-based general absolute value function */
template <typename Type>
    requires requires(Type a) {
        { a > Type{0} } -> std::convertible_to<bool>;
        { -a } -> std::constructible_from<Type>;
    }
inline Type unit_abs(const Type &val)
{
    return (val > Type{0}) ? (val) : -val;
}