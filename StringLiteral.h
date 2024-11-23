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

    // Commenting these out until we use them; no need for an overly complex implementation

    // consteval bool operator==(const StringLiteral<N> str) const
    // {
    //     return std::equal(str.data, str.data + N, data);
    // }

    // template <std::size_t N2>
    // consteval bool operator==(const StringLiteral<N2>) const
    // {
    //     return false;
    // }

    // consteval char operator[](std::size_t n) const {
    //     return data[n];
    // }

    // consteval std::size_t size() const
    // {
    //     return N - 1;
    // }
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

/** Equality helper */
// template<StringLiteral Str1, StringLiteral Str2>
// struct IsSameLiteralHelper
// {
//     static constexpr bool value = Str1.value == Str2.value;
// };

// template<StringLiteral Str1, StringLiteral Str2>
// using IsSameLiteral = IsSameLiteralHelper<Str1, Str2>::value;

/** Type trait */
// This is just a wrapper since StringLiteral<"MyStr"> will give an error saying "MyStr" isn't type size_t

template <StringLiteral Str>
struct MakeStrLitHelper
{
    static constexpr StringLiteral str = Str;
};


template <StringLiteral Str>
using MakeStrLit = typename MakeStrLitHelper<Str>::str;


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
 
