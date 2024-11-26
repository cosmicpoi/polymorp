#pragma once

#include <type_traits>
#include <ratio>
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
    requires std::is_arithmetic_v<typename A::type>;
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

template <typename Type, IsRatio Ratio, typename RHS_Type, IsRatio RHS_Ratio>
constexpr bool typed_ratio_equality(Type value, RHS_Type rhs)
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
    CommonType val2 = (rhs * fac2);
    return std::abs(val1 - val2) < epsilon;
}

template <UnitLike From, UnitLike To>
    requires UnitIsConvertible_<From, To>
typename To::type resolve_ratio_assignment(const From &fromVal)
{
    typename From::type product = MultByRatio<typename From::ratio, typename From::type>(fromVal.GetValue());
    typename To::type value = DivideByRatio<typename To::ratio, typename To::type>(product);
    return value;
}

/** @brief Unit definition */
template <typename Type, UnitIdentifier UID = EmptyUid, IsRatio Ratio = std::ratio<1>>
    requires std::is_arithmetic_v<Type> && (Ratio::num > 0)
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
    explicit inline Unit(T val)
        : value(val){};

    /** @brief Constructor for converting from like units */
    template <UnitIsConvertible_<ThisType> UnitT>
    explicit inline Unit(UnitT val)
        : value(resolve_ratio_assignment<UnitT, ThisType>(val)){};

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
    inline ThisType &operator=(const RHS &rhs)
    {
        if constexpr (UnitSameRatio_<ThisType, RHS>)
        {
            value = rhs.GetValue();
            return *this;
        }
        else
        {
            value = resolve_ratio_assignment<RHS, ThisType>(rhs);
            return *this;
        }
    }

    template <typename T>
        requires IsEmptyUid<UID> && std::is_arithmetic_v<T> && std::is_convertible_v<T, Type>
    inline ThisType &operator=(const T &rhs)
    {
        value = DivideByRatio<Ratio>(rhs);
        return *this;
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
        requires CanMultiply<Type, RHS_Type>
    inline UnitMultBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator*(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return UnitMultBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{value * rhs.GetValue()};
    }

    /** @brief Multiply with unitless scalar. */
    template <std::convertible_to<Type> RHS>
        requires CanMultiply<Type, RHS>
    inline UseWithScalar_<RHS> operator*(const RHS &rhs) const
    {
        return UseWithScalar_<RHS>{value * rhs};
    }

    /** @brief Divide by another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanDivide<Type, RHS_Type>
    inline UnitDivBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator/(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return UnitDivBy_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{value / rhs.GetValue()};
    }

    /** @brief Divide by unitless scalar. */
    template <std::convertible_to<Type> RHS>
        requires CanDivide<Type, RHS>
    inline UseWithScalar_<RHS> operator/(const RHS &rhs) const
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
        requires std::is_same_v<UID, typename RHS::uid>
    using UnitAdd_ = Unit<
        std::common_type_t<Type, typename RHS::type>,
        UID,
        CombineRatio<Ratio, typename RHS::ratio>>;

    /** @brief Add with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioAdd<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator+(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{
            (((ResType)value) / (Ratio::den * RHS_Ratio::num)) + (((ResType)rhs.GetValue()) / (RHS_Ratio::den * Ratio::num)) //
        };
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioSubtract<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator-(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
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
        requires requires(ThisType a, T b) { a * b; a = a * b; }
    inline ThisType &operator*=(const T &rhs)
    {
        value = ThisType{(*this) * rhs}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a / b; a = a / b; }
    inline ThisType &operator/=(const T &rhs)
    {
        value = ThisType{*this / rhs}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a + b; a = a + b; }
    inline ThisType &operator+=(const T &rhs)
    {
        value = ThisType{*this + rhs}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a - b; a = a - b; }
    inline ThisType &operator-=(const T &rhs)
    {
        value = ThisType{*this - rhs}.value;
        return *this;
    }

    /** Comparison */

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            A <=> B;
        }
    inline auto operator<=>(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        constexpr auto fac1 = Ratio::num * RHS_Ratio::den;
        constexpr auto fac2 = RHS_Ratio::num * Ratio::den;

        Type val1 = (value * fac1);
        RHS_Type val2 = (rhs.GetValue() * fac2);

        return val1 <=> val2;
    }

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            { A == B } -> std::convertible_to<bool>;
        }
    inline bool operator==(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return typed_ratio_equality<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue());
    }

    template <typename T>
        requires requires(Type a, T b) {
            requires IsEmptyUid<UID>;
            requires std::is_arithmetic_v<T>;
            { a == b } -> std::convertible_to<bool>;
        }
    inline bool operator==(const T &rhs)
    {
        return typed_ratio_equality<Type, Ratio, T, std::ratio<1>>(value, rhs);
    }

    // template <typename RHS>

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
    requires std::is_arithmetic_v<Type>
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
template <IsUnit U, intmax_t Exp>
using UnitExpI = UnitExp<U, std::ratio<Exp>>;

// Multiply an existing unit by a ratio
template <IsUnit U, IsRatio Ratio>
using UnitMultRatio = Unit<
    typename U::type,
    typename U::uid,
    std::ratio_multiply<Ratio, typename U::ratio>>;

// Empty unit shorthand
template <typename T>
    requires std::is_arithmetic_v<T>
using EmptyUnit = Unit<T, EmptyUid>;

// Other operator overloads
template <typename Type, UnitIdentifier UID, IsRatio Ratio>
std::ostream &operator<<(std::ostream &os, Unit<Type, UID, Ratio> val)
{
    return val.operator<<(os);
}

/** @breif Left-compare with plain type for EmptyUnits */
template <typename LHS, IsUnit Unit_RHS>
    requires requires(typename Unit_RHS::type a, LHS b) {
        requires IsEmptyUid<typename Unit_RHS::uid>;
        requires std::is_arithmetic_v<LHS>;
        { a == b } -> std::convertible_to<bool>;
    }
inline bool operator==(const LHS &lhs, const Unit_RHS &rhs)
{
    return rhs == lhs;
}

/** @brief Left-multiply by scalar */
template <typename LHS, IsUnit Unit_RHS>
    requires requires(LHS a, Unit_RHS b) { b.operator*(a); }
inline auto operator*(const LHS &lhs, const Unit_RHS &rhs)
{
    return rhs.operator*(lhs);
}

/** @brief Left-divide with scalar */
template <typename LHS, IsUnit Unit_RHS>
    requires requires(LHS a, UnitExpI<Unit_RHS, -1> b) { b.operator*(a); }
inline auto operator/(const LHS &lhs, const Unit_RHS &rhs)
{
    UnitExpI<Unit_RHS, -1> rhs_inv{1 / rhs.GetValue()};
    return rhs_inv.operator*(lhs);
}

/**
 * Template shorthands
 */

template <typename T, StringLiteral Symbol>
    requires std::is_arithmetic_v<T>
using TypeAtomic = Unit<T, MakeUnitIdentifier<UnitAtomic<Symbol>>>;

template <StringLiteral Symbol>
using dAtomic = TypeAtomic<double, Symbol>;

template <StringLiteral Symbol>
using fAtomic = TypeAtomic<float, Symbol>;

template <StringLiteral Symbol>
using iAtomic = TypeAtomic<int, Symbol>;