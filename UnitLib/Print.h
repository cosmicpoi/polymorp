#pragma once

#include "Unit.h"
#include "Vector.h"
#include "Matrix.h"
#include "TypeUtils.h"

#include <iostream>
#include <cxxabi.h>

// ----------------------------------------------------
/// @TODO: delete this stuff in prod code

// Helper to demangle type names
constexpr std::string demangle(const char *name)
{
    int status = 0;
    char *demangled = abi::__cxa_demangle(name, nullptr, nullptr, &status);
    std::string result = (status == 0) ? demangled : name;
    free(demangled);
    return result;
}

template <typename T>
constexpr std::string GetDemangledType()
{
    return demangle(typeid(T).name());
}

template <typename T>
constexpr void PrintTypeInfo(std::ostream &os = std::cout)
{
    os << "is const: " << std::is_const_v<T> << std::endl;
    os << GetDemangledType<T>() << std::endl;
}
// ----------------------------------------------------

/** @brief Function to print std::ratio */
template <IsRatio T>
constexpr void PrintRatio(std::ostream &os = std::cout)
{
    os << T::num;
    if (T::den != 1)
    {
        os << "/" << T::den;
    }
}

/** @brief Print StringLiteral */
template <StringLiteral Str>
void PrintStrLit(std::ostream &os = std::cout)
{
    os << Str.data;
}

template <typename T>
static constexpr inline void PrintInfo(std::ostream &os = std::cout)
{
    if constexpr (IsUnitLeaf<T>)
    {
        PrintStrLit<T::symbol>(os);
        os << " ";
        PrintRatio<typename T::exponent>(os);
    }
    else if constexpr (IsUnitLeafVector<T>)
    {
        [&]<typename... Ts>(std::tuple<Ts...>)
        {
            ([&]
             { PrintInfo<Ts>(os); os << "; "; }(), ...);
        }(ExtractParameterPack<T>{});
    }
    else if constexpr (IsUnit<T>)
    {
        os << GetDemangledType<typename T::type>() << " ( ";
        PrintInfo<typename T::uid>(os);
        os << ") X";
        PrintRatio<typename T::ratio>(os);
        os << "; ";
    }
    else if constexpr (IsVector<T>)
    {
        PrintInfo<typename T::type>();
        os << "[N=" << (T::n) << "];";
    }
    else if constexpr (IsMatrix<T>)
    {
        PrintInfo<typename T::type>();
        os << "[M=" << (T::m) << ", N=" << (T::n) << "];";
    }
    else
    {
        os << GetDemangledType<T>() << "; ";
    }
}

template <typename T>
inline void Print(T val, std::ostream &os = std::cout)
{
    if constexpr (IsUnit<T>)
    {
        os << "u{ ";
        PrintInfo<T>(os);
        os << " " << val.GetValue() << " }";
    }
    else if constexpr (IsVector<T>)
    {
        os << "v{ ";
        PrintInfo<T>(os);
        os << " (";
        for (uint i = 0; i < T::n; i++)
        {
            // check for operator<< and use it
            if constexpr (IsUnit<typename T::type>)
            {
                os << val[i].GetValue();
            }
            else if constexpr (requires(typename T::type a, std::ostream &o) { o << a; })
            {
                os << val[i];
            }
            else
            {
                os << GetDemangledType<T>();
            }

            if (i != T::n - 1)
            {
                os << " ";
            }
        }
        os << ") }";
    }
    else if constexpr (IsMatrix<T>)
    {
        os << "m{{ ";
        PrintInfo<T>(os);
        os << std::endl;
        for (uint i = 0; i < T::m; i++)
        {
            os << "  ";
            for (uint j = 0; j < T::n; j++)
            {
                if constexpr (IsUnit<typename T::type>)
                {
                    os << val[i][j].GetValue();
                }
                else if constexpr (requires(typename T::type a, std::ostream &o) { o << a; })
                {
                    os << val[i][j];
                }
                else
                {
                    os << GetDemangledType<T>();
                }

                if (j != T::n - 1)
                {
                    os << " ";
                }
            }
            if (i == T::m - 1)
            {
                os << " }}";
            }
            os << std::endl;
        }
    }
    else
    {
        PrintTypeInfo<T>(os);
    }
}

template <typename T>
concept UserDefinedPrint = IsUnit<T> || IsVector<T> || IsMatrix<T>;

template <UserDefinedPrint T>
std::ostream &operator<<(std::ostream &os, const T &val)
{
    Print<T>(val, os);
    return os;
}
