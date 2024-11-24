#pragma once

#include <type_traits>
#include <ratio>
#include "UnitIdentifier.h"
#include "TypeUtils.h"

template <typename Type, UnitIdentifier UID, IsRatio Ratio = std::ratio<1>>
struct Unit
{
public:
    // Use the default constructor of the underlying type
    Unit() {};

    // Type traits
    using type = Type;
    using uid = UID;
    using ratio = Ratio;

    /** @brief Print type info */
    static constexpr void PrintInfo()
    {
        std::cout << typeid(Type).name() << " ( ";
        UID::Print();
        std::cout << ") x";
        PrintRatio<Ratio>();
        std::cout << "; ";
    }

    /** @brief Print type info and value (no newline) */
    void Print()
    {
        PrintInfo();
        std::cout << " " << value;
    }

    /** @brief Logging shorthand to print a header, contents, and newline */
    void Log()
    {
        std::cout << "Unit print" << std::endl;
        Print();
        std::cout << std::endl;
    }

    /** @brief Constructor for converting from literal */
    Unit(Type value)
        : value(value) {};

    /** @brief Multiply with another unit. If the types clash, always convert to the LHS unit. */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    auto operator*(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
    {
        return Unit<Type, UIMult<UID, RHS_UID>, std::ratio_multiply<Ratio, RHS_Ratio>>{(Type)(value * rhs.value)};
    }

    /** @brief Divide by another unit. If the types clash, always convert to the LHS unit. */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    auto operator/(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
    {
        return Unit<Type, UIDivide<UID, RHS_UID>, std::ratio_divide<Ratio, RHS_Ratio>>{(Type)(value / rhs.value)};
    }

    /** @brief Add with another unit, only if UIDs match. If the types clash, always convert to the LHS unit. */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    auto operator+(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
        requires(std::is_same_v<UID, RHS_UID>)
    {
        // TODO support ratio properly
        return Unit<Type, UID, Ratio>{(Type)(value + rhs.value)};
    }

    /** @brief Subtract with another unit, only if UIDs match. If the types clash, always convert to the LHS unit. */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    auto operator-(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
        requires(std::is_same_v<UID, RHS_UID>)
    {
        // TODO support ratio properly
        return Unit<Type, UID, Ratio>{(Type)(value - rhs.value)};
    }

    /** @brief Underlying value */
    Type value = 0;
};

// Concept to match Unit
template <typename T>
struct IsUnitHelper : std::false_type
{
};

// Specialization for the `Unit` template
template <typename Type, UnitIdentifier UID, IsRatio Ratio>
struct IsUnitHelper<Unit<Type, UID, Ratio>> : std::true_type
{
};

template <typename T>
concept IsUnit = IsUnitHelper<T>::value;

