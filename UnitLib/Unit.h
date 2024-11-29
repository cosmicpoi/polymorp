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

/**
 * Helpers for difficult ratio math
 */

template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
struct RatioEqualityHelper
{
    using CommonType = std::common_type_t<LHS_Type, RHS_Type>;

    static constexpr intmax_t fac1 = LHS_Ratio::num * RHS_Ratio::den;
    static constexpr intmax_t fac2 = RHS_Ratio::num * LHS_Ratio::den;
    static constexpr intmax_t facMax = fac1 > fac2 ? fac1 : fac2;
    static constexpr CommonType eps1 = std::numeric_limits<LHS_Type>::epsilon();
    static constexpr CommonType eps2 = std::numeric_limits<RHS_Type>::epsilon();
    static constexpr CommonType eps = eps1 > eps2 ? eps1 : eps2;

    static constexpr CommonType epsilon = eps * facMax * EPS_TOLERANCE;
};

// Used for equality
template <typename Type, IsRatio Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires(RHS_Ratio::num > 0 && Ratio::num > 0 )
constexpr bool typed_ratio_equality(Type value, RHS_Type rhs)
{
    // It might be tempting to add a check for if constexpr (std::is_same_v<Ratio, RHS_Ratio>), but
    // we want to override the default eps tolerance even for plain types (since they may have converted from Kilo)
    using Helper = RatioEqualityHelper<Type, Ratio, RHS_Type, RHS_Ratio>;
    using CommonType = typename Helper::CommonType;
    return std::abs(static_cast<CommonType>(value * Helper::fac1) - static_cast<CommonType>(rhs * Helper::fac2)) < Helper::epsilon;
}

/**
 * @brief Ratio addition/subtraction helper.
 * Supports expressions of the form `V1 * (N1/D1) + V2 * (N2/D2)`.
 */
template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
std::common_type_t<LHS_Type, RHS_Type> ratio_value_add(const LHS_Type &lhs, const RHS_Type &rhs)
{
    using ResType = std::common_type_t<LHS_Type, RHS_Type>;
    using LhsFac = typename CombineRatio<LHS_Ratio, RHS_Ratio>::lhsFac;
    using RhsFac = typename CombineRatio<LHS_Ratio, RHS_Ratio>::rhsFac;

    return MultiplyByRatio<LhsFac, ResType>(lhs) + MultiplyByRatio<RhsFac, ResType>(rhs);
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

    /** @brief Constructor for converting from literal */
    template <std::convertible_to<Type> T>
    explicit inline Unit(T val)
        : value(val){};

    /** @brief Constructor for converting from like units */
    template <UnitIsConvertible_<ThisType> UnitT>
    explicit inline Unit(const UnitT &val)
        : value(DivideByRatio<Ratio, Type>(val.GetRealValue())){};

    /** @brief Construct from rvalue of like unit */
    template <UnitIsConvertible_<ThisType> UnitT>
    explicit inline Unit(const UnitT &&val) : Unit(val) {}

    // Check is zero
    inline bool IsZero() const
    {
        return value == 0 || Ratio::num == 0;
    }

    // Value getters
    inline Type &GetValue() { return value; }
    inline const Type &GetValue() const { return value; }

    /** @brief Compute the real value from the ratio */
    inline const Type GetRealValue() const
    {
        return MultiplyByRatio<Ratio, Type>(value);
    }
    /** @brief Compute the value in terms of base units */
    inline const Unit<Type, UID, std::ratio<1>> GetBaseUnitValue() const
    {
        return Unit<Type, UID, std::ratio<1>>{GetRealValue()};
    }

    /**
     * Assignment operators
     */
    template <UnitIsConvertible_<ThisType> RHS>
        requires(!std::is_same_v<ThisType, RHS>)
    inline ThisType &operator=(const RHS &rhs)
    {
        if constexpr (UnitSameRatio_<ThisType, RHS>)
        {
            value = rhs.GetValue();
            return *this;
        }
        else
        {
            value = DivideByRatio<Ratio, Type>(rhs.GetRealValue());
            return *this;
        }
    }

    template <typename T>
        requires IsEmptyUid<UID> && std::is_arithmetic_v<T> && std::is_convertible_v<T, Type>
    inline ThisType &operator=(const T &rhs)
    {
        value = DivideByRatio<Ratio, Type>(rhs);
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
        typename CombineRatio<Ratio, typename RHS::ratio>::combinedRatio>;

    /** @brief Add with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioAdd<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator+(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{
            ratio_value_add<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue())};
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioSubtract<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>> operator-(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return UnitAdd_<Unit<RHS_Type, RHS_UID, RHS_Ratio>>{
            ratio_value_add<Type, Ratio, RHS_Type, RHS_Ratio>(value, -rhs.GetValue())};
    }

    /** @brief Add with a unitless scalar, if we are empty */
    template <std::convertible_to<Type> RHS>
        requires CanAdd<Type, RHS> && IsEmptyUid<UID>
    inline UseWithScalar_<RHS> operator+(const RHS &rhs) const
    {
        // TODO fix with ratios
        return UseWithScalar_<RHS>{
            ratio_value_add<Type, Ratio, RHS, std::ratio<1>>(value, rhs)};
    }

    /** @brief Subtract with a unitless scalar, if we are empty */
    template <std::convertible_to<Type> RHS>
        requires CanSubtract<Type, RHS> && IsEmptyUid<UID>
    inline UseWithScalar_<RHS> operator-(const RHS &rhs) const
    {
        return UseWithScalar_<RHS>{
            ratio_value_add<Type, Ratio, RHS, std::ratio<1>>(value, -rhs)};
    }

    /** @brief Unary negation */
    inline ThisType operator-() const
    {
        return -1 * (*this);
    }

    /**
     * Proxy compute-and-assign operators: +=, -=, *=, /=
     */
    template <typename T>
        requires requires(ThisType a, T b) { a = a * b; }
    inline ThisType &operator*=(const T &rhs)
    {
        value = ThisType{(*this) * rhs}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a = a / b; }
    inline ThisType &operator/=(const T &rhs)
    {
        value = ThisType{*this / rhs}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a = a + b; }
    inline ThisType &operator+=(const T &rhs)
    {
        value = ThisType{*this + rhs}.value;
        return *this;
    }

    template <typename T>
        requires requires(ThisType a, T b) { a = a - b; }
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

    // Compare with another unit of the same UID (different type and ratio OK)
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            { A == B } -> std::convertible_to<bool>;
        }
    inline bool operator==(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return typed_ratio_equality<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue());
    }

    // Compare with empty unit
    template <typename T>
        requires requires(Type a, T b) {
            requires IsEmptyUid<UID>;
            requires std::is_arithmetic_v<T>;
            { a == b } -> std::convertible_to<bool>;
        }
    inline bool operator==(const T &rhs) const
    {
        return typed_ratio_equality<Type, Ratio, T, std::ratio<1>>(value, rhs);
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

template <IsUnit A, IsUnit B>
using UnitDivide = decltype(std::declval<A>() / std::declval<B>());

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

/** @breif Left-compare with plain type for EmptyUnits */
template <typename LHS, IsUnit Unit_RHS>
    requires requires(Unit_RHS a, LHS b) {
        requires IsEmptyUid<typename Unit_RHS::uid>;
        requires std::is_arithmetic_v<LHS>;
        { a.operator==(b) } -> std::convertible_to<bool>;
    }
inline bool operator==(const LHS &lhs, const Unit_RHS &rhs)
{
    return rhs.operator==(lhs);
}

/** @brief Left-multiply by scalar */
template <typename LHS, IsUnit Unit_RHS>
    requires CanOpMultiply<Unit_RHS, LHS>
inline OpMultiplyType<Unit_RHS, LHS> operator*(const LHS &lhs, const Unit_RHS &rhs)
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

/** @brief Left-add with scalar */
template <typename LHS, IsUnit Unit_RHS>
    requires CanOpAdd<Unit_RHS, LHS>
OpAddType<Unit_RHS, LHS> operator+(LHS lhs, Unit_RHS rhs)
{
    return rhs.operator+(lhs);
}

template <typename LHS, IsUnit Unit_RHS>
    requires CanOpAdd<Unit_RHS, LHS>
OpAddType<Unit_RHS, LHS> operator-(LHS lhs, Unit_RHS rhs)
{
    return (-1 * rhs).operator+(lhs);
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