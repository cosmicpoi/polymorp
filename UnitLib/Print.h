#include "Unit.h"
#include "Vector.h"
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
constexpr void PrintTypeInfo(std::ostream &os = std::cout)
{
    os << "is const: " << std::is_const_v<T> << std::endl;
    os << demangle(typeid(T).name()) << std::endl;
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
static constexpr inline void PrintInfo<T>(std::ostream &os = std::cout)
{
    if constexpr (IsUnitLeaf<T>)
    {
        PrintStrLit<typename T::symbol>(os);
        os << " ";
        PrintRatio<typename T::exponent>(os);
    }
    else if constexpr (IsUnitLeafVector<T>)
    {
        [&]<typename... Ts>(std::tuple<Ts...>)
        {
            ([&]
             { Print<Ts>(os); os << "; "; }(), ...);
        }(ExtractParameterPack<T>{});
    }
    else if constexpr (IsUnit<T>)
    {
        os << typeid(Type).name() << " ( ";
        Print<typename T::uid>(os);
        os << ") X";
        PrintRatio<typename T::ratio>(os);
        os << "; ";
    }
    else if constexpr (IsVector<T>)
    {
        PrintInfo<typename T::type>();
        os << "[N=" << (T::n) << "];";
    }
    else
    {
        os << demangle(typeid(T).name()) << "; ";
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
                os << _v[i].GetValue();
            }
            else if constexpr (requires(typename T::type a, std::ostream &o) { a.operator<<(o); })
            {
                os << _v[i];
            }

            if (i != T::n - 1)
            {
                os << " ";
            }
        }
        os << ") }";
    }
    else
    {
        PrintInfo<T>(os);
    }
}

template <typename T>
inline void Log(T val, std::ostream &os = std::cout) if constexpr (IsUnit<T>)
{
    if constexpr (IsUnit<T>)
    {
        os << "Unit print" << std::endl;
        Print<T>(val, os);
        os << std::endl;
    }
    else if constexpr (IsVector<T>)
    {
        // TODO
    }
    else
    {
        Print<T>(val, os);
        // check if has operator << and use it?
        os << std::endl;
    }
}

// struct HasPrint
// {
//     inline std::ostream &operator<<(std::ostream &os) const
//     {
//         Print(os);
//         return os;
//     }
// }