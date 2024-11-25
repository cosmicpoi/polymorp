#pragma once

#include <type_traits>
#include <ratio>
#include "UnitIdentifier.h"
#include "TypeUtils.h"

/** Helper concepts */
template <typename A>
concept UnitLike = requires {
    typename A::uid;
    typename A::ratio;
    typename A::type;
    requires IsRatio<typename A::ratio>;
    requires UnitIdentifier<typename A::uid>;
};

template <typename A, typename B>
concept SameUid_ = requires {
    requires UnitLike<A>;
    requires UnitLike<B>;
    requires std::is_same_v<typename A::uid, typename B::uid>;
};

template <typename A, typename B>
concept UnitSameRatio_ = requires {
    requires UnitLike<A>;
    requires UnitLike<B>;
    requires std::is_same_v<typename A::ratio, typename B::ratio>;
};

template <typename From, typename To>
concept UnitIsConvertible_ = requires {
    requires UnitLike<From>;
    requires UnitLike<To>;
    requires SameUid_<From, To>;
    requires std::is_convertible_v<typename From::type, typename To::type>;
};

/** @brief Unit definition */
template <typename Type, UnitIdentifier UID = EmptyUid, IsRatio Ratio = std::ratio<1>>
struct Unit
{
public:
    // Use the default constructor of the underlying type
    Unit() {};

    // Type traits
    using ThisType = Unit<Type, UID, Ratio>;
    using type = Type;
    using uid = UID;
    using ratio = Ratio;

    /** @brief Print type info */
    static constexpr void PrintInfo(std::ostream &os = std::cout)
    {
        os << typeid(Type).name() << " ( ";
        UID::Print(os);
        os << ") X";
        PrintRatio<Ratio>(os);
        os << "; ";
    }

    /** @brief Print type info and value (no newline) */
    inline void Print(std::ostream &os = std::cout) const
    {
        os << "u{ ";
        PrintInfo(os);
        os << " " << value << " }";
    }

    /** @brief Logging shorthand to print a header, contents, and newline */
    inline void Log(std::ostream &os) const
    {
        os << "Unit print" << std::endl;
        Print(os);
        os << std::endl;
    }

    /** @brief Constructor for converting from literal */
    template <std::convertible_to<Type> T>
    inline Unit(T val)
        : value(val){};

    inline bool IsZero() const
    {
        return value == 0 || Ratio::num == 0;
    }

    /**
     * Copy assignments
     */
    template <UnitIsConvertible_<ThisType> RHS>
    inline ThisType &operator=(RHS rhs)
    {
        if constexpr (UnitSameRatio_<ThisType, RHS>)
        {
            value = rhs.value;
            return *this;
        }
        else
        {
            Type product = RatioMult<typename RHS::ratio>(rhs.value);
            value = RatioDiv<Ratio>(product);
            return *this;
        }
    }

    /**
     *  Multiplication and Division
     */

    /** @brief Multiply with another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline auto operator*(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return Unit<ResType, UIMult<UID, RHS_UID>, std::ratio_multiply<Ratio, RHS_Ratio>>{value * rhs.value};
    }

    /** @brief Multiply with unitless scalar. */
    template <std::convertible_to<Type> RHS>
    inline auto operator*(RHS rhs) const
    {
        using ResType = std::common_type_t<Type, RHS>;
        return operator*(Unit<ResType, EmptyUid>{rhs});
    }

    /** @brief Divide by another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline auto operator/(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return Unit<ResType, UIDivide<UID, RHS_UID>, std::ratio_divide<Ratio, RHS_Ratio>>{value / rhs.value};
    }

    /** @brief Divide by unitless scalar. */
    inline auto operator/(Type rhs) const
    {
        return Unit<Type, UID, Ratio>{value / rhs};
    }

    /**
     * Addition and subtraction
     *
     *    These can be surprisingly tricky because we need to properly handle ratios.
     *    The overall method is to preserve as much info as possible in the ratio even
     *    if it becomes nonsensical, and rely on copy assignment to coerce the ratio
     *    into the one the user actually wants. For instance:
     *      Kilometer v = Kilometer{1} + Meter{1}; // expect 1.001km, not 1001m
     */

    /** @brief Add with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline auto operator+(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires(std::is_same_v<UID, RHS_UID>)
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return Unit<ResType, UID, CombineRatio<Ratio, RHS_Ratio>>{
            (value / (Ratio::den * RHS_Ratio::num)) + (rhs.value / (RHS_Ratio::den * Ratio::num)) //
        };
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline auto operator-(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires(std::is_same_v<UID, RHS_UID>)
    {
        return operator+(rhs * -1);
    }

    /**
     * Proxy compute-and-assign operators: +=, -=, *=, /=
     */
    template <typename T>
        requires requires(ThisType a, T b) { a * b; a = b; a = a * b; }
    inline ThisType &operator*=(T rhs)
    {
        value = ThisType{operator*(rhs)}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a / b; a = b; a = a / b; }
    inline ThisType &operator/=(T rhs)
    {
        value = ThisType{operator/(rhs)}.value;
        return *this;
    }

    /** Comparison */

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline auto operator<=>(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            A <=> B;
        }
    {
        auto diff = *this - rhs;
        return diff.value <=> 0;
    }

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline bool operator==(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            { A == B } -> std::convertible_to<bool>;
        }
    {
        return value * Ratio::num * RHS_Ratio::den == rhs.value * RHS_Ratio::num * Ratio::den;
    }

    std::ostream &operator<<(std::ostream &os) const
    {
        Print(os);
        return os;
    }

    /** @brief Underlying value */
    Type value = 0;
};

// Concept to match Unit
template <typename T>
struct IsUnitHelper : std::false_type
{
};

template <typename Type, UnitIdentifier UID, IsRatio Ratio>
struct IsUnitHelper<Unit<Type, UID, Ratio>> : std::true_type
{
};

template <typename T>
concept IsUnit = IsUnitHelper<T>::value;

// Stronger versions of underscored aliases above to check for Unit. We have to do it this way because of declaration order
template <typename A, typename B>
concept SameUid = IsUnit<A> && IsUnit<B> && SameUid_<A, B>;

template <typename A, typename B>
concept UnitSameRatio = IsUnit<A> && IsUnit<B> && UnitSameRatio_<A, B>;

template <typename From, typename To>
concept UnitIsConvertible = IsUnit<From> && IsUnit<To> && UnitIsConvertible_<From, To>;

// Other operator overloads
template <typename Type, UnitIdentifier UID, IsRatio Ratio>
std::ostream &operator<<(std::ostream &os, Unit<Type, UID, Ratio> val)
{
    return val.operator<<(os);
}

/** @brief Left-multiply by scalar */
template <typename LHS, IsUnit Unit_RHS>
    requires requires(LHS a, Unit_RHS b) { b.operator*(a); }
auto operator*(LHS lhs, Unit_RHS rhs)
{
    return rhs * lhs;
}

/** @brief Left-divide with scalar */
template <typename LHS, IsUnit Unit_RHS>
    requires requires(LHS a, Unit_RHS b) { a.operator*(b); }
auto operator/(LHS lhs, Unit_RHS rhs)
{
    return lhs * unit_pow<std::ratio<-1>>(rhs);
}

// Get the resulant unit from multiplying two units. Left-side type always dominates.
template <IsUnit A, IsUnit B>
using UnitMult = decltype(std::declval<A>() * std::declval<B>());

// Get the resulting unit from adding two units of type A and B.
// Note this doesn't actually "add units" (because what does that even mean),
// it just computes the type of Unit A + Unit B.
template <IsUnit A, IsUnit B>
using UnitAdd = decltype(std::declval<A>() + std::declval<B>());

// In principle this should be the same as add, but if we get zero values
// weird things can happen
template <IsUnit A, IsUnit B>
using UnitSubtract = decltype(std::declval<A>() + std::declval<B>());

// Get type for exponentiated unit
template <IsUnit U, IsRatio Exp>
using UnitExp = Unit<
    typename U::type,
    UIExp<typename U::uid, Exp>,
    typename U::ratio>;

// Shorthand for int exponents
template <IsUnit U, int Exp>
using UnitExpI = UnitExp<U, std::ratio<Exp>>;

// Multiply an existing unit by a ratio
template <IsUnit U, IsRatio Ratio>
using UnitMultRatio = Unit<
    typename U::type,
    typename U::uid,
    std::ratio_multiply<Ratio, typename U::ratio>>;

/**
 * Some template shorthands for easier deduction
 */

template <typename T, StringLiteral Symbol>
using TypeAtomic = Unit<T, MakeUnitIdentifier<UnitAtomic<Symbol>>>;

template <StringLiteral Symbol>
using dAtomic = TypeAtomic<double, Symbol>;

template <StringLiteral Symbol>
using fAtomic = TypeAtomic<float, Symbol>;

template <StringLiteral Symbol>
using iAtomic = TypeAtomic<int, Symbol>;