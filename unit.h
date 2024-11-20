#pragma once

#include <string>
#include <iostream>

/// @todo Switch from int Exponent to std::ratio Exponent

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
    Unit(Type value)
        : value(value) {};
    void Print()
    {
        std::cout << Symbol << std::endl;
    }

    /**
     * @brief Multiply with a unit with the same type and symbol (and any exponent)
     */
    template <char RHS_Symbol, typename RHS_Type, int RHS_Exponent>
    auto operator*(Unit<RHS_Symbol, RHS_Type, RHS_Exponent> rhs_unit)
    {
        static_assert(Symbol == RHS_Symbol, "Unit symbols must match");
        static_assert(std::is_same_v<Type, RHS_Type>, "Unit types must match");

        return Unit<Symbol, Type, Exponent + RHS_Exponent>{value * rhs_unit.value};
    }

    /**
     * @brief Add with a unit with the same type and symbol (and any exponent)
     */
    template <char RHS_Symbol, typename RHS_Type, int RHS_Exponent>
    auto operator+(Unit<RHS_Symbol, RHS_Type, RHS_Exponent> rhs_unit)
    {
        static_assert(Symbol == RHS_Symbol, "Unit symbols must match");
        static_assert(std::is_same_v<Type, RHS_Type>, "Unit types must match");

        return Unit<Symbol, Type, Exponent>{value + rhs_unit.value};
    }

    /**
     * @brief Subtract with a unit with the same type and symbol (and any exponent)
     */
    template <char RHS_Symbol, typename RHS_Type, int RHS_Exponent>
    auto operator-(Unit<RHS_Symbol, RHS_Type, RHS_Exponent> rhs_unit)
    {
        static_assert(Symbol == RHS_Symbol, "Unit symbols must match");
        static_assert(std::is_same_v<Type, RHS_Type>, "Unit types must match");

        return Unit<Symbol, Type, Exponent>{value - rhs_unit.value};
    }

    /**
     * @brief Divide with a unit with the same type and symbol (and any exponent)
     */
    template <char RHS_Symbol, typename RHS_Type, int RHS_Exponent>
    auto operator/(Unit<RHS_Symbol, RHS_Type, RHS_Exponent> rhs_unit)
    {
        static_assert(Symbol == RHS_Symbol, "Unit symbols must match");
        static_assert(std::is_same_v<Type, RHS_Type>, "Unit types must match");

        return Unit<Symbol, Type, Exponent - RHS_Exponent>{value / rhs_unit.value};
    }

private:
    Type value;
};

using Meter = Unit<'m', double, 1>;
using Meter_2 = Unit<'m', double, 2>;

inline Meter operator"" _meter(long double value)
{
    return Meter{static_cast<double>(value)};
}