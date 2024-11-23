// Adapted from https://dev.to/sgf4/strings-as-template-parameters-c20-4joh
#pragma once

#include <algorithm>

template<std::size_t N>
struct StringLiteral
{
    char data[N]{};

    consteval StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, data);
    }
};

// Compare two StringLiterals lexicographically at compile-time

constexpr bool const_strcmp(const char* str1, const char* str2) {
    size_t idx = 0;
    while (str1[idx] && str1[idx] == str2[idx])
    {
        idx++;
    }
    
    // Return the difference between the ASCII values of the mismatched characters
    return str1[idx] - str2[idx] > 0 ? true : false;
}

// Specialization for comparing two StringLiteral types
template <StringLiteral T1, StringLiteral T2>
struct CompareStrings
{
    static constexpr bool value = const_strcmp(T1.data, T2.data);
};


 /** Print helper function */
 // https://ctrpeach.io/posts/cpp20-string-literal-template-parameters/

template<StringLiteral Str>
void PrintStrLit() {
    std::cout << Str.data;
}

template<StringLiteral Str>
void PrintStrLit_Endl() {
    PrintStrLit<Str>();
    std::cout << std::endl;
}

/** Type trait */
// This is just a wrapper since StringLiteral<"MyStr"> will give an error saying "MyStr" isn't type size_t

template <StringLiteral Str>
struct MakeStrLitHelper
{
    static constexpr StringLiteral str = Str;
};


template <StringLiteral Str>
using MakeStrLit = typename MakeStrLitHelper<Str>::str;

// Helpers for sorting


// Helper for getting underlying data from string lit
// template <StringLiteral Str>
// struct StrLitGetHelper
// {
//     static constexpr const char* str = Str.data;
// };


// template <StringLiteral Str>
// const char* StrLitGet()
// {
//     return StrLitGetHelper<Str>::str;
// }
 
