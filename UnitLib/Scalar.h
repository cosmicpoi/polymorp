#pragma once

#include "Unit.h"

// GeneralScalar - generalized concept to represent anything that can be multiplied by a unit
template <typename T>
concept PlainScalar = !IsUnit<T> && std::is_arithmetic_v<T>;
template <typename T>
concept GeneralScalar = IsUnit<T> || PlainScalar<T>; //

template <GeneralScalar T, IsRatio Exp>
struct ScalarExp_
{
};

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

// Get type of underlying value
template <GeneralScalar T>
struct GetUnderlying_
{
};

template <IsUnit U>
struct GetUnderlying_<U>
{
    using type = typename U::type;
};

template <PlainScalar T>
struct GetUnderlying_<T>
{
    using type = T;
};

template <GeneralScalar T>
using GetUnderlying = typename GetUnderlying_<T>::type;


// Get the value
template <GeneralScalar T>
const GetUnderlying<T> ScalarGetValue(const T val)
{
    if constexpr (IsUnit<T>)
    {
        return val.GetValue();
    }
    else
    {
        return val;
    }
}

// Conversion helper

template <typename From, typename To>
concept ScalarIsConvertible =
    (IsUnit<From> && IsUnit<To> && UnitIsConvertible<From, To>) ||
    (PlainScalar<From> && PlainScalar<To> && std::is_convertible_v<From, To>);
