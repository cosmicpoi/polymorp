//--------------------------------------------------------------------------------
// CompoundUnit.h
//
//   Support for building compound units (product and division)
//--------------------------------------------------------------------------------

#pragma once

template <char Symbol, int Exponent>
struct UnitBase
{
    static constexpr char symbol = Symbol;
    static constexpr int exponent = Exponent;
};

// Define the primary template
template <typename T>
struct IsUnitBaseHelper : std::false_type
{};

// Specialization for the `Unit` template
template <char Symbol, int Exponent>
struct IsUnitBaseHelper<UnitBase<Symbol, Exponent>> : std::true_type
{};

/** 
 * @brief Concept to check if a type is a Unit
 */
template <typename T>
concept IsUnitBase = IsUnitBaseHelper<T>::value;



template<typename... Ts> requires (IsUnitBase<Ts> && ...)
struct UnitProduct
{
    static constexpr size_t n = sizeof...(Ts);
    static constexpr char units[ n ];
};