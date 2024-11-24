#pragma once

#include <concepts>

/** Concept to ensure ::Print() exists */
template <typename T>
concept HasPrint = requires {
    { T::Print() } -> std::same_as<void>;
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

/** Function to print std::ratio */
template <IsRatio T>
constexpr void PrintRatio()
{
    std::cout << T::num;
    if (T::den != 1)
    {
        std::cout << "/" << T::den;
    }
}