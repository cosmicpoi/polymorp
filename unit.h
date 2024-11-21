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
        std::cout << symbol << exponent << std::endl;
    }

    // type traits accessible at runtime
    static constexpr char symbol = Symbol;
    using type = Type;
    static constexpr int exponent = Exponent;



    /** 
     * @brief Constructor for converting from literal
     */
    Unit(Type value)
        : value(value) {

            std::cout << "that function is running!" << std::endl;
        };

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

// Helper type trait to extract template parameters from Unit
template <typename U>
struct UnitTraits;

// Specialization for the Unit class
template <char Symbol, typename Type, int Exponent>
struct UnitTraits<Unit<Symbol, Type, Exponent>> {
    static constexpr char symbol = Symbol;
    using type = Type;
    static constexpr int exponent = Exponent;
};

/** @brief Concept to check if a type is a specialization of Unit
 * 
 */
// Define the primary template
template <typename T>
struct IsUnitHelper : std::false_type {};

// Specialization for the `Unit` template
template <char Symbol, typename Type, int Exponent>
struct IsUnitHelper<Unit<Symbol, Type, Exponent>> : std::true_type {};

// Define the concept
template <typename T>
concept IsUnit = IsUnitHelper<T>::value;