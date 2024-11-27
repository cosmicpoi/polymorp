#pragma once

#include <concepts>
#include <ratio>

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

/** Function to print std::ratio */
template <IsRatio T>
constexpr void PrintRatio(std::ostream &os = std::cout)
{
    os << T::num;
    if (T::den != 1)
    {
        os << "/" << T::den;
    }
}

/** Function to multiply out a ratio: Compute val * R */
template <IsRatio R, typename T>
T MultByRatio(T val)
{
    if (R::num == 1 && R::den == 1)
    {
        return val;
    }
    return (T)(val * ((T)R::num) / ((T)R::den));
}

/** Function to divide out a ratio* Compute val / R */
template <IsRatio R, typename T>
T DivideByRatio(T val)
{
    if (R::num == 1 && R::den == 1)
    {
        return val;
    }
    return (T)(val * ((T)R::den) / ((T)R::num));
};

/** Convert ratio to double */
template <IsRatio R>
constexpr double RatioAsDouble()
{
    return ((double)R::num) / ((double)R::den);
}

/** Helper function for adding/subtracting ratios */
template <IsRatio R1, IsRatio R2>
struct CombineRatio_
{
    using ratio = std::ratio<R1::num * R2::num>;
};

template <IsRatio R1, IsRatio R2>
using CombineRatio = typename CombineRatio_<R1, R2>::ratio;

/** Invert a ratio */
template <IsRatio R>
using RatioInvert = std::ratio<R::den, R::num>;

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
    using _PosExp = std::ratio<-1* Exp::num, Exp::den>;
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
