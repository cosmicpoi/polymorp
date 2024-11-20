#pragma once

#include <string>
#include <iostream>

/**
 * @brief base unit class which holds a type of type Type
 * 
 * @tparam Symbol unique char representing the unit
 * @tparam Type underlying type of the unit
 * @tparam Exponent exponent of unit (e.g. 2 for `m^2`)
 */
template <char Symbol, typename Type, int Exponent>
class Unit
{   
public:
    Unit(double value)
        : value(value) {};
    void Print()
    {
        std::cout << Symbol << std::endl;
    }
    Type value;
};

using Meter = Unit<'m', double, 1>;
using Meter_2 = Unit<'m', double, 2>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}