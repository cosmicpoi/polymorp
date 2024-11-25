#pragma once

#include <type_traits>
#include <ratio>
#include <iomanip>
#include "_Consts.h"
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

    // Check is zero
    inline bool IsZero() const
    {
        return value == 0 || Ratio::num == 0;
    }

    // Value getters
    inline Type &GetValue() { return value; }
    inline const Type &GetValue() const { return value; }

    /**
     * Copy assignments
     */
    template <UnitIsConvertible_<ThisType> RHS>
    inline ThisType &operator=(RHS rhs)
    {
        if constexpr (UnitSameRatio_<ThisType, RHS>)
        {
            value = rhs.GetValue();
            return *this;
        }
        else
        {
            Type product = MultByRatio<typename RHS::ratio>(rhs.GetValue());
            value = DivideByRatio<Ratio>(product);
            return *this;
        }
    }

    /**
     *  Multiplication and Division
     */

    // Helper types
    template <UnitLike RHS>
    using UnitMultBy_ = Unit<
        std::common_type_t<Type, typename RHS::type>,
        UIMult<UID, typename RHS::uid>,
        std::ratio_multiply<Ratio, typename RHS::ratio>>;

    template <UnitLike RHS>
    using UnitDivBy_ = Unit<
        std::common_type_t<Type, typename RHS::type>,
        UIDivide<UID, typename RHS::uid>,
        std::ratio_divide<Ratio, typename RHS::ratio>>;

    template <typename RHS>
    using UseWithScalar_ = Unit<std::common_type_t<Type, RHS>, UID, Ratio>;

    /** @brief Multiply with another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline UnitMultBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator*(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires CanMultiply<Type, RHS_Type>
    {
        return UnitMultBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{value * rhs.GetValue()};
    }

    /** @brief Multiply with unitless scalar. */
    template <std::convertible_to<Type> RHS>
    inline UseWithScalar_<RHS> operator*(RHS rhs) const
        requires CanMultiply<Type, RHS>
    {
        return UseWithScalar_<RHS>{value * rhs};
    }

    /** @brief Divide by another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline UnitDivBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator/(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires CanDivide<Type, RHS_Type>
    {
        return UnitDivBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{value / rhs.GetValue()};
    }

    /** @brief Divide by unitless scalar. */
    template <std::convertible_to<Type> RHS>
    inline UseWithScalar_<RHS> operator/(RHS rhs) const
        requires CanDivide<Type, RHS>
    {
        return UseWithScalar_<RHS>{value / rhs};
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

    // Helper for adding OR subtracting
    template <UnitLike RHS>
    using UnitAdd_ = Unit<
        std::common_type_t<Type, typename RHS::type>,
        UID, // requires is_same_v<UID, RHS::uid>
        CombineRatio<Ratio, typename RHS::ratio>>;

    /** @brief Add with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator+(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires CanRatioAdd<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{
            (((ResType)value) / (Ratio::den * RHS_Ratio::num)) + (((ResType)rhs.GetValue()) / (RHS_Ratio::den * Ratio::num)) //
        };
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator-(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires CanRatioSubtract<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{
            (((ResType)value) / (Ratio::den * RHS_Ratio::num)) - (((ResType)rhs.GetValue()) / (RHS_Ratio::den * Ratio::num)) //
        };
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

    template <typename T>
        requires requires(ThisType a, T b) { a + b; a = b; a = a + b; }
    inline ThisType &operator+=(T rhs)
    {
        value = ThisType{operator+(rhs)}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a - b; a = b; a = a - b; }
    inline ThisType &operator-=(T rhs)
    {
        value = ThisType{operator-(rhs)}.value;
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
        constexpr auto fac1 = Ratio::num * RHS_Ratio::den;
        constexpr auto fac2 = RHS_Ratio::num * Ratio::den;

        Type val1 = (value * fac1);
        RHS_Type val2 = (rhs.GetValue() * fac2);

        return val1 <=> val2;
    }

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline bool operator==(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs) const
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            { A == B } -> std::convertible_to<bool>;
        }
    {
        using CommonType = std::common_type_t<Type, RHS_Type>;

        constexpr auto fac1 = Ratio::num * RHS_Ratio::den;
        constexpr auto fac2 = RHS_Ratio::num * Ratio::den;
        constexpr auto facMax = fac1 > fac2 ? fac1 : fac2;
        constexpr CommonType eps1 = std::numeric_limits<Type>::epsilon();
        constexpr CommonType eps2 = std::numeric_limits<RHS_Type>::epsilon();
        constexpr CommonType eps = eps1 > eps2 ? eps1 : eps2;

        constexpr CommonType epsilon = eps * facMax * EPS_TOLERANCE;

        CommonType val1 = (value * fac1);
        CommonType val2 = (rhs.GetValue() * fac2);
        return std::abs(val1 - val2) < epsilon;
    }

    std::ostream &operator<<(std::ostream &os) const
    {
        Print(os);
        return os;
    }

private:
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
using UnitSubtract = decltype(std::declval<A>() - std::declval<B>());

// Check if unit U can actually be exponentiated by ratio Exp
template <typename U, typename Exp>
concept UnitExpableRatio = requires {
    requires IsUnit<U>;
    requires IsRatio<Exp>;
    typename U::ratio;
    requires IsRatio<typename U::ratio>;
    requires RatioCanExp<typename U::ratio, Exp>;
};

template <typename, typename>
struct UnitExp_
{
};

// Get type for exponentiated unit
template <IsUnit U, IsRatio Exp>
    // requires UnitExpableRatio<U, Exp>
struct UnitExp_<U, Exp>
{
    using type = Unit<
        typename U::type,
        UIExp<typename U::uid, Exp>,
        typename RatioExp<typename U::ratio, Exp>::value>;
};

template <IsUnit U, IsRatio Exp>
    requires UnitExpableRatio<U, Exp>
using UnitExp = typename UnitExp_<U, Exp>::type;

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