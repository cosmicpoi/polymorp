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
    return (T)(val * ((T)R::num) / ((T)R::den));
}

/** Function to divide out a ratio* Compute val / R */
template <IsRatio R, typename T>
T DivideByRatio(T val)
{
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
using CombineRatio = std::ratio<R1::num * R2::num>;

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
constexpr intmax_t IntPow() {
    return IntPow_<Base, Exponent>::value;
}

// Define static pow of ratio with intmax_t
template <IsRatio Ratio, intmax_t Exponent>
struct RatioPow_
{
    static const intmax_t _num = IntPow<Ratio::num, Exponent>();
    static const intmax_t _den = IntPow<Ratio::den, Exponent>();
    using type = std::ratio<_num, _den>;
};

template <IsRatio Ratio>
struct RatioPow_<Ratio, 0>
{
    using type = std::ratio<1>;
};


// template <IsRatio Ratio, IsRatio Exp>
// using RatioExpI_ = std::ratio <

//                    struct RatioExp<IsRatio Ratio, IsRatio Exp>
// {
// };