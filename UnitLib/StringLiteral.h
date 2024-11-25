// Adapted from https://dev.to/sgf4/strings-as-template-parameters-c20-4joh
#pragma once

#include <iostream>
#include <algorithm>

template <std::size_t N>
struct StringLiteral
{
    constexpr static size_t n = N;
    char data[N]{};

    consteval StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, data);
    }
};

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

/** Print helper function */
// https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

template <StringLiteral Str>
void PrintStrLit(std::ostream &os = std::cout)
{
    os << Str.data;
}

/** Type trait */
// This is just a wrapper since StringLiteral<"MyStr"> will give an error saying "MyStr" isn't type size_t

template <StringLiteral Str>
struct MakeStrLit
{
    static constexpr StringLiteral str = Str;
};