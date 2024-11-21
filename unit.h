#pragma once

#include <string>
#include <iostream>

//--------------------------------------------------------------------------------
// Unit class
//--------------------------------------------------------------------------------

/// @todo Switch from int Exponent to std::ratio Exponent
/// @todo Support ratios

/**
 * @brief Base unit class which holds a type of type Type
 *
 * @tparam Symbol unique char representing the unit
 * @tparam Type underlying type of the unit
 * @tparam Exponent exponent of unit (e.g. 2 for `m^2`)
 */
template <char Symbol, typename Type, int Exponent>
class Unit
{
public:
    /// @todo Test this
    // Use the default constructor of the underlying type
    Unit() {};
    // Debug function
    void Print()
    {
        std::cout << "Unit print" << std::endl;
        std::cout << symbol << exponent << " " << value << std::endl;
    }

    // type traits accessible at runtime
    static constexpr char symbol = Symbol;
    using type = Type;
    static constexpr int exponent = Exponent;

    /**
     * @brief Constructor for converting from literal
     */
    Unit(Type value)
        : value(value) {};

    /**
     * @brief Get the underlying represented value
     */
    Type GetValue()
    {
        return value;
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

//--------------------------------------------------------------------------------
// Type helpers
//--------------------------------------------------------------------------------

/* ******* Concept ********* */

// Define the primary template
template <typename T>
struct IsUnitHelper : std::false_type
{};

// Specialization for the `Unit` template
template <char Symbol, typename Type, int Exponent>
struct IsUnitHelper<Unit<Symbol, Type, Exponent>> : std::true_type
{};

/** 
 * @brief Concept to check if a type is a Unit
 */
template <typename T>
concept IsUnit = IsUnitHelper<T>::value;

/* ******* Type traits ********* */

/** 
 * @brief Concept to check if a type is a specialization of Unit
 */
template <IsUnit U>
struct UnitTraits
{
    // Extract the parameters directly from U using partial specialization
    static constexpr char symbol = U::symbol;
    using type = typename U::type;
    static constexpr int exponent = U::exponent;
};

//--------------------------------------------------------------------------------
// Type transformers
//--------------------------------------------------------------------------------

/* ******* Exponentiators ********* */

/**
 * @brief Adjust exponent by by Delta (i.e. if Delta=1, `m^2` -> `m^3`)
 */
template <IsUnit U, int Delta>
using AdjustExponent = Unit<U::symbol, typename U::type, U::exponent + Delta>;

/**
 * @brief Exponentiate by 1 (i.e. `m^2` -> `m^3`)
 */
template <IsUnit U>
using Exp1 = AdjustExponent<U, 1>;

/**
 * @brief De-exponentiate by 1 (i.e. `m^2` -> `m^1`)
 */
template <IsUnit U>
using DExp1 = AdjustExponent<U, -1>;


/* ******* Casting helpers ********* */
