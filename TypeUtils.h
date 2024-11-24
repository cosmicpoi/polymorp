#pragma once

#include <concepts>
#include <iostream>

/** Concept to ensure ::Print() exists */
template <typename T>
concept HasPrint = requires(std::ostream& os) {
    { T::Print(os) } -> std::same_as<void>;
};

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
    T::num == 0;
} && T::num == 0;

template <typename T>
concept RatioIsZero = IsRatio<T> && RatioIsZero_<T>;

/** Function to print std::ratio */
template <IsRatio T>
constexpr void PrintRatio(std::ostream& os = std::cout)
{
    os << T::num;
    if (T::den != 1)
    {
        os << "/" << T::den;
    }
}

/** Function to multiply out a ratio */
template <typename T, IsRatio R>
T MultRatio(T val)
{
    return (T)(val * ((T)R::num) / ((T)R::den));
}

template <typename T, IsRatio R>
T DivRatio(T val)
{
    return (T)(val * ((T)R::den) / ((T)R::num));
};

/** Convert ratio to double */
template <IsRatio R>
constexpr double RatioAsDouble()
{
    return ((double)R::num) / ((double)R::den);
}