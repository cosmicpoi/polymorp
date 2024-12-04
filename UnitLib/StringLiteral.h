// Adapted from https://dev.to/sgf4/strings-as-template-parameters-c20-4joh
#pragma once

#include <iostream>
#include <algorithm>

template <size_t N>
struct StringLiteral
{
    constexpr static size_t n = N;
    char data[N]{};

    consteval StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, data);
    }
};

// // Concept for stringliterals
// template <typename T>
// struct IsStringLiteralHelper : std::false_type
// {
// };

// template <size_t N>
// struct IsStringLiteralHelper<StringLiteral<N>> : std::true_type
// {
// };

// /** @brief Concept to check if a type is a StringLiteral */
// template <typename T>
// concept IsStringLiteral = IsStringLiteralHelper<T>::value;

// Compare two StringLiterals lexicographically at compile-time

constexpr int const_strcmp(const char *str1, const char *str2)
{
    size_t idx = 0;
    while (str1[idx] && str1[idx] == str2[idx])
    {
        idx++;
    }

    // Return the difference between the ASCII values of the mismatched characters
    return str2[idx] - str1[idx];
}

// Comparing two StringLiteral types
template <StringLiteral T1, StringLiteral T2>
struct CompareStrings
{
    static constexpr bool value = const_strcmp(T1.data, T2.data) > 0;
};

//
template <StringLiteral T1, StringLiteral T2>
struct StrEq
{
    static constexpr bool value = (const_strcmp(T1.data, T2.data) == 0) && (decltype(T1)::n == decltype(T2)::n);
};

/** Type trait */
// This is just a wrapper since StringLiteral<"MyStr"> will give an error saying "MyStr" isn't type size_t

template <StringLiteral Str>
struct MakeStrLit
{
    static constexpr StringLiteral str = Str;
};