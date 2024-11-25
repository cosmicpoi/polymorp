#pragma once

#include "Unit.h"

// GeneralScalar - generalized concept to represent anything that can be multiplied by a unit
template <typename T>
concept PrimitiveScalar = !IsUnit<T> && (std::convertible_to<T, int>       //
                                         || std::convertible_to<T, float>  //
                                         || std::convertible_to<T, double> //
                                         || std::convertible_to<T, long>);
template <typename T>
concept GeneralScalar = IsUnit<T> || PrimitiveScalar<T>; //

template <GeneralScalar T, IsRatio Exp>
struct ScalarExp_
{
};

template <IsUnit U, IsRatio Exp>
struct ScalarExp_<U, Exp>
{
    using type = UnitExp<U, Exp>;
};

template <PrimitiveScalar T, IsRatio Exp>
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

template <PrimitiveScalar T>
struct GetUnderlying_<T>
{
    using type = T;
};

template <GeneralScalar T>
using GetUnderlying = typename GetUnderlying_<T>::type;

// General print info
template <GeneralScalar T>
void ScalarPrintInfo(std::ostream &os = std::cout)
{
    if constexpr (IsUnit<T>)
    {
        T::PrintInfo(os);
    }
    else
    {
        os << typeid(T).name() << "; ";
    }
}

// Get the value
template <GeneralScalar T>
GetUnderlying<T> ScalarGetValue(T val)
{
    if constexpr (IsUnit<T>)
    {
        return val.value;
    }
    else
    {
        return val;
    }
}