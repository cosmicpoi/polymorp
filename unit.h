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
// Concepts
//--------------------------------------------------------------------------------

/* ******* Basic Concept ********* */

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


/* ******* Pairwise unit checking ********* */

/**
 * @brief Concept that matches units of the same symbol and type, but possibly different exponent (e.g. a float `m` and float `m^2`)
 */
template<typename U, typename V>
concept BothUnits = IsUnit<U> && IsUnit<V>;

template<typename U, typename V>
concept IsSameSymbol = requires
{
    { V::symbol } -> std::same_as<const char&>;
    { U::symbol } -> std::same_as<const char&>;
    { V::symbol == U::symbol } -> std::same_as<bool>;
} && (V::symbol == U::symbol);

template<typename U, typename V>
concept IsSameType = requires
{
    typename V::type;
    typename U::type;
} && std::same_as<typename V::type, typename U::type>;

template<typename U, typename V>
concept IsSameUnit = BothUnits<U, V> && IsSameSymbol<U, V> && IsSameType<U, V>;


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

/**
 * @brief Double the exponent of U
 */
template <IsUnit U>
using DoubleExp = AdjustExponent<U, U::exponent>;

template <int N>
concept IsEven = (N % 2 == 0);
/**
 * @brief Half the exponent of U (can only be used on even numbers)
 */
template <IsUnit U> requires IsEven<U::exponent>
using HalfExp = AdjustExponent<U, -U::exponent / 2>;