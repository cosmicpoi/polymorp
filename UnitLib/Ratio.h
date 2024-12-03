#pragma once

#include <concepts>
#include <ratio>
#include "TypeUtils.h"

/** Concept to match ratio */

// Define the trait
template <typename>
struct IsRatio_ : std::false_type
{
};

template <std::intmax_t Num, std::intmax_t Den>
struct IsRatio_<std::ratio<Num, Den>> : std::true_type
{
};

// Define the concept using `is_ratio_v`
template <typename T>
concept IsRatio = IsRatio_<T>::value;

/** Concept to check for zero ratio */
template <typename T>
concept RatioIsZero_ = requires {
    { T::num } -> std::convertible_to<int>;
    requires T::num == 0;
};

template <typename T>
concept RatioIsZero = IsRatio<T> && RatioIsZero_<T>;

/** Invert a ratio */
template <IsRatio R>
using RatioInvert = std::ratio<R::den, R::num>;

/**
 * Check if the given type can be used with non-trivial (i.e. 1/1) ratios
 * Essentially, requires closure under multiplication and division by intmax_t.
 * This is, notably, a stronger requirement than we typically use in most of the
 * Unit checks, where we'd otherwise split this into e.g. CanRatioMultiply and CanRatioDivide.
 * The reason is that we do a lot of ratio manipulation logic under the hood for
 * many operations, e.g. if we assign from ratio<16> to ratio<1> we divide the real
 * value by 16, so mult and div both need to be defined.
 *
 * I have a hard time thinking of a real use-case for a type that can only be
 * mult by intmax and can't be divided. If one comes up we can try to split this.
 */
// TODO remove the ConvertibleOrConstructibleTo requirement. In theory it's OK if 
// e.g. it produces a new type as long as MultByRatio(LHS) and MultByRatio(RHS) prodcue the same type
template <typename T>
concept IsRatioCompatible_ = requires(T t, intmax_t i) {
    { t / i } -> ConvertibleOrConstructibleTo<T>;
    { t *i } -> ConvertibleOrConstructibleTo<T>;
};

template <typename... Ts>
concept IsRatioCompatible = ((IsRatioCompatible_<Ts> && ...));

/** Function to multiply out a ratio: Compute val * R */
template <IsRatio R, typename OutType, typename T>
    requires ConvertibleOrConstructible<OutType, T> &&
             (IsRatioCompatible<T> || std::is_same_v<R, std::ratio<1>>)
OutType MultiplyByRatio(const T &val)
{
    if constexpr (!IsRatioCompatible<T>)
    {
        static_assert(std::is_same_v<R, std::ratio<1>>);
        return val;
    }
    else if constexpr (std::is_same_v<R, std::ratio<1>>)
    {
        return ConvertOrConstruct<OutType>(val);
    }
    else
    {
        return ConvertOrConstruct<OutType>(val * (ConvertOrConstruct<OutType>(R::num)) / ConvertOrConstruct<OutType>(R::den));
    }
}

/** Function to divide out a ratio* Compute val / R */
template <IsRatio R, typename OutType, typename T>
    requires IsRatioCompatible<T> &&
             (IsRatioCompatible<T> || std::is_same_v<R, std::ratio<1>>)
OutType DivideByRatio(const T &val)
{
    return MultiplyByRatio<RatioInvert<R>, OutType, T>(val);
};

/** Convert ratio to double */
template <IsRatio R>
constexpr double RatioAsDouble()
{
    return static_cast<double>(R::num) / static_cast<double>(R::den);
}

/**
 * @brief Helper to find the common ratio between two ratios when adding/subtracting, namely, their LCM.
 *      1/1000 + 1/1 -> 1/1
 *      1 + 16 -> 16
 *      1/2 + 1/3 -> 1
 *      2/3 + 1/4 -> 2
 * Namely, lcm(N1/D1, N2/D2) = lcm(N1, N2)/gcd(D1, D2)
 */
constexpr intmax_t gcd(intmax_t a, intmax_t b)
{
    if (b == 0)
    {
        return a;
    }
    return gcd(b, a % b);
}

constexpr intmax_t lcm(intmax_t a, intmax_t b)
{
    return (a * b) / gcd(a, b);
}

template <IsRatio R1, IsRatio R2>
struct RatioAddHelper
{
    static constexpr intmax_t _combinedNum = lcm(R1::num, R2::num);
    static constexpr intmax_t _combinedDen = gcd(R1::den, R2::den);
    using combinedRatio = std::ratio<_combinedNum, _combinedDen>;
    // using lhsFac = std::ratio<1, R1::den * R2::num>;
    using lhsFac = std::ratio_divide<R1, combinedRatio>;
    using rhsFac = std::ratio_divide<R2, combinedRatio>;
};

template <typename SharedType, typename Ratio, typename RHS_Ratio>
struct TypedRatioAddHelper
{
};

template <typename SharedType, typename Ratio, typename RHS_Ratio>
    requires IsRatioCompatible<SharedType>
struct TypedRatioAddHelper<SharedType, Ratio, RHS_Ratio> : RatioAddHelper<Ratio, RHS_Ratio>
{
};

template <typename SharedType, typename Ratio, typename RHS_Ratio>
    requires(!IsRatioCompatible<SharedType>)
struct TypedRatioAddHelper<SharedType, Ratio, RHS_Ratio>
{
    using combinedRatio = std::ratio<1>;
};

/** Helper for exponentiating ratios: compute Ratio ^ Exp (when possible) */
// Reference: https://stackoverflow.com/questions/19823216/stdratio-power-of-a-stdratio-at-compile-time

// Define static pow of two intmax_t
template <intmax_t Base, intmax_t Exponent>
struct IntPow_
{
    static const intmax_t _temp = IntPow_<Base, Exponent / 2>::value;
    static const intmax_t value = _temp * _temp * (Exponent % 2 == 1 ? Base : 1);
};

template <intmax_t Base>
struct IntPow_<Base, 0>
{
    static const intmax_t value = 1;
};

template <intmax_t Base, intmax_t Exponent>
constexpr intmax_t IntPow()
{
    return IntPow_<Base, Exponent>::value;
}

// Define static pow of ratio with intmax_t
template <IsRatio Ratio, intmax_t Exponent>
struct RatioPowI_
{
    static const intmax_t _num = IntPow<Ratio::num, Exponent>();
    static const intmax_t _den = IntPow<Ratio::den, Exponent>();
    using type = std::ratio<_num, _den>;
};

template <IsRatio Ratio>
struct RatioPowI_<Ratio, 0>
{
    using type = std::ratio<1>;
};

template <IsRatio Ratio, intmax_t Exponent>
using RatioPowI = typename RatioPowI_<Ratio, Exponent>::type;

// Check whether Root^N = Val
template <intmax_t Val, intmax_t Root, intmax_t N>
constexpr bool IsIntegralRoot()
{
    return (IntPow<Root, N>() == Val);
}

// Compute greatest possible root, i.e highest Root s.t. Root^N <= Val
//    Algorithm is essentially binary search on [1, 1 + Val / N] (Val^(1/N) <= Val / N)
//    If the root is non-integral, terminates on ceil(root)
template <intmax_t Val, intmax_t N, intmax_t Left, intmax_t Right>
constexpr intmax_t _HighestRoot()
{
    // std::cout << Left << " " << Right << std::endl;
    if (Left == Right)
    {
        return Left;
    }

    constexpr intmax_t midpoint = (Left + Right) / 2;
    constexpr intmax_t res = IntPow<midpoint, N>();
    if constexpr (res == Val)
    {
        return midpoint;
    }
    else if constexpr (res > Val)
    {
        return _HighestRoot<Val, N, Left, midpoint>();
    }
    else
    {
        return _HighestRoot<Val, N, midpoint + 1, Right>();
    }

    return 0;
}

template <intmax_t Val, intmax_t N>
    requires(N > 0)
constexpr intmax_t HighestRoot()
{
    if (N == 1)
    {
        return Val;
    }
    return _HighestRoot<Val, N, 1, 1 + Val / N>();
}

template <intmax_t Val, intmax_t N>
concept HasIntegralRoot = (IntPow<HighestRoot<Val, N>(), N>() == Val);

template <IsRatio Ratio, IsRatio Exp>
    requires(Ratio::num > 0) && (Exp::num > 0)
struct RatioExp_
{
    using _temp = RatioPowI<Ratio, Exp::num>;
    static constexpr bool _numHasRoot = HasIntegralRoot<_temp::num, Exp::den>;
    static constexpr bool _denHasRoot = HasIntegralRoot<_temp::den, Exp::den>;
    static constexpr intmax_t _tempNum = HighestRoot<_temp::num, Exp::den>();
    static constexpr intmax_t _tempDen = HighestRoot<_temp::den, Exp::den>();

    // Public values
    static constexpr bool hasValue = _numHasRoot && _denHasRoot;
    using value = std::ratio<_tempNum, _tempDen>;
};

template <typename, typename>
struct RatioExp
{
};

template <IsRatio Ratio, IsRatio Exp>
    requires(Exp::num == 0)
struct RatioExp<Ratio, Exp>
{
    static constexpr bool hasValue = true;
    using value = std::ratio<1>;
};

template <IsRatio Ratio, IsRatio Exp>
    requires(Exp::num > 0)
struct RatioExp<Ratio, Exp>
{
    static constexpr bool hasValue = RatioExp_<Ratio, Exp>::hasValue;
    using value = typename RatioExp_<Ratio, Exp>::value;
};

template <IsRatio Ratio, IsRatio Exp>
    requires(Exp::num < 0)
struct RatioExp<Ratio, Exp>
{
    using _Inv = RatioInvert<Ratio>;
    using _PosExp = std::ratio<-1 * Exp::num, Exp::den>;
    using _Res = RatioExp_<_Inv, _PosExp>;
    static constexpr bool hasValue = _Res::hasValue;
    using value = typename _Res::value;
};

template <IsRatio Ratio, IsRatio Exp>
constexpr bool RatioCanExp_()
{
    return RatioExp<Ratio, Exp>::hasValue;
}

template <typename Ratio, typename Exp>
concept RatioCanExp = requires {
    requires IsRatio<Ratio>;
    requires IsRatio<Exp>;
} && RatioCanExp_<Ratio, Exp>();
