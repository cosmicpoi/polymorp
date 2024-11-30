#pragma once

#include <type_traits>
#include <ratio>
#include "_Consts.h"
#include "UnitIdentifier.h"
#include "TypeUtils.h"

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
    requires(RHS_Ratio::num > 0 && Ratio::num > 0)
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

    /**
     * @brief Constructor from convertible unit with same UID
     * Note: need to check that either type or ratio does not match to avoid overriding copy constructor
     */
    template <typename Other_Type, UnitIdentifier Other_UID, IsRatio Other_Ratio>
        requires(std::is_same_v<UID, Other_UID> &&
                 ConvertibleOrConstructible<Type, Other_Type> &&
                 (!std::is_same_v<ThisType, Unit<Other_Type, Other_UID, Other_Ratio>>))
    explicit inline Unit(const Unit<Other_Type, Other_UID, Other_Ratio> &val)
        : value(DivideByRatio<Ratio, Type>(ConvertOrConstruct<Type, Other_Type>(val.GetRealValue()))){};

    /**
     * @brief Constructor from assignable unit with same UID
     * Note: need to check that either type or ratio does not match to avoid overriding copy constructor
     */
    template <typename Other_Type, UnitIdentifier Other_UID, IsRatio Other_Ratio>
        requires(std::is_same_v<UID, Other_UID> &&
                 (!ConvertibleOrConstructible<Type, Other_Type>) &&
                 AssignableTo<Type, Other_Type> &&
                 (!std::is_same_v<ThisType, Unit<Other_Type, Other_UID, Other_Ratio>>))
    explicit inline Unit(const Unit<Other_Type, Other_UID, Other_Ratio> &val)
    {
        value = DivideByRatio<Ratio, Type>(val.GetRealValue());
    }

    /**
     * @brief Constructor from constructible or convertible rvalue
     * Note: need to check that ratio does not match to avoid overriding copy constructor
     */
    template <typename Other_Type, UnitIdentifier Other_UID, IsRatio Other_Ratio>
        requires(std::is_same_v<UID, Other_UID> &&
                 ConvertibleOrConstructible<Type, Other_Type> &&
                 (!std::is_same_v<Type, Other_Type>))
    explicit inline Unit(const Unit<Other_Type, Other_UID, Other_Ratio> &&val)
        : Unit(val)
    {
    }

    /**
     * @brief Constructor froma ssignable or convertible rvalue
     * Note: need to check that ratio does not match to avoid overriding copy constructor
     */
    template <typename Other_Type, UnitIdentifier Other_UID, IsRatio Other_Ratio>
        requires(std::is_same_v<UID, Other_UID> &&
                 (!ConvertibleOrConstructible<Type, Other_Type>) &&
                 AssignableTo<Type, Other_Type> &&
                 (!std::is_same_v<Type, Other_Type>))
    explicit inline Unit(const Unit<Other_Type, Other_UID, Other_Ratio> &&val)
        : Unit(val)
    {
    }

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

    /**
     * @brief Assign from unit with convertible or constructible underlying type
     * Note: either ratio or type has to be different, so that we don't override copy constructor
     */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires(std::is_same_v<UID, RHS_UID> &&
                 ConvertibleOrConstructible<Type, RHS_Type> &&
                 (!std::is_same_v<ThisType, Unit<RHS_Type, RHS_UID, RHS_Ratio>>))
    inline ThisType &operator=(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
    {
        if constexpr (std::is_same_v<Ratio, RHS_Ratio>)
        {
            value = ConvertOrConstruct<Type, RHS_Type>(rhs.GetValue());
            return *this;
        }
        else
        {
            value = ConvertOrConstruct<Type, RHS_Type>(DivideByRatio<Ratio, Type>(rhs.GetRealValue()));
            return *this;
        }
    }

    /**
     * @brief Assign from unit with assignable underlying type unit
     * Note: either ratio or type has to be different, so that we don't override copy constructor
     */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires(std::is_same_v<UID, RHS_UID> &&
                 !ConvertibleOrConstructible<Type, RHS_Type> &&
                 AssignableTo<Type, RHS_Type> &&
                 (!std::is_same_v<ThisType, Unit<RHS_Type, RHS_UID, RHS_Ratio>>))
    inline ThisType &operator=(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
    {
        if constexpr (std::is_same_v<Ratio, RHS_Ratio>)
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

    /** @brief For empty units, assign from a convertible or constructible plain scalar */
    template <typename RHS_Type>
        requires IsEmptyUid<UID> &&
                 std::is_arithmetic_v<RHS_Type> &&
                 ConvertibleOrConstructible<Type, RHS_Type>
    inline ThisType &operator=(const RHS_Type &rhs)
    {
        if constexpr (std::is_same_v<Ratio, std::ratio<1>>)
        {
            value = ConvertOrConstruct<Type, RHS_Type>(rhs);
            return *this;
        }
        else
        {
            value = ConvertOrConstruct<Type, RHS_Type>(DivideByRatio<Ratio, Type>(rhs));
            return *this;
        }
        return *this;
    }

    /** @brief For empty units, assign from an assignable plain scalar */
    template <typename RHS_Type>
        requires IsEmptyUid<UID> &&
                 std::is_arithmetic_v<RHS_Type> &&
                 (!ConvertibleOrConstructible<Type, RHS_Type>) &&
                 AssignableTo<Type, RHS_Type>
    inline ThisType &operator=(const RHS_Type &rhs)
    {
        if constexpr (std::is_same_v<Ratio, std::ratio<1>>)
        {
            value = rhs;
            return *this;
        }
        else
        {
            value = DivideByRatio<Ratio, Type>(rhs);
            return *this;
        }
        return *this;
    }

    /**
     *  Multiplication and Division
     */

    // Helper types
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    using ThisUnitMultiply_ = Unit<
        std::common_type_t<Type, RHS_Type>,
        UIMult<UID, RHS_UID>,
        std::ratio_multiply<Ratio, RHS_Ratio>>;

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    using ThisUnitDivide__ = Unit<
        std::common_type_t<Type, RHS_Type>,
        UIDivide<UID, RHS_UID>,
        std::ratio_divide<Ratio, RHS_Ratio>>;

    template <typename RHS>
    using UseWithPlaintype_ = Unit<std::common_type_t<Type, RHS>, UID, Ratio>;

    /** @brief Multiply with another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanMultiply<Type, RHS_Type>
    inline ThisUnitMultiply_<RHS_Type, RHS_UID, RHS_Ratio> operator*(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitMultiply_<RHS_Type, RHS_UID, RHS_Ratio>{value * rhs.GetValue()};
    }

    /** @brief Multiply with unitless scalar. */
    template <std::convertible_to<Type> RHS>
        requires CanMultiply<Type, RHS>
    inline UseWithPlaintype_<RHS> operator*(const RHS &rhs) const
    {
        return UseWithPlaintype_<RHS>{value * rhs};
    }

    /** @brief Divide by another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanDivide<Type, RHS_Type>
    inline ThisUnitDivide__<RHS_Type, RHS_UID, RHS_Ratio> operator/(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitDivide__<RHS_Type, RHS_UID, RHS_Ratio>{value / rhs.GetValue()};
    }

    /** @brief Divide by unitless scalar. */
    template <std::convertible_to<Type> RHS>
        requires CanDivide<Type, RHS>
    inline UseWithPlaintype_<RHS> operator/(const RHS &rhs) const
    {
        return UseWithPlaintype_<RHS>{value / rhs};
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
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID>
    using ThisUnitAdd_ = Unit<
        std::common_type_t<Type, RHS_Type>,
        UID,
        typename CombineRatio<Ratio, RHS_Ratio>::combinedRatio>;

    /** @brief Add with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioAdd<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio> operator+(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio>{
            ratio_value_add<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue())};
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioSubtract<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio> operator-(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio>{
            ratio_value_add<Type, Ratio, RHS_Type, RHS_Ratio>(value, -rhs.GetValue())};
    }

    /** @brief Add with a unitless scalar, if we are empty */
    template <std::convertible_to<Type> RHS>
        requires CanAdd<Type, RHS> && IsEmptyUid<UID>
    inline UseWithPlaintype_<RHS> operator+(const RHS &rhs) const
    {
        return UseWithPlaintype_<RHS>{
            ratio_value_add<Type, Ratio, RHS, std::ratio<1>>(value, rhs)};
    }

    /** @brief Subtract with a unitless scalar, if we are empty */
    template <std::convertible_to<Type> RHS>
        requires CanSubtract<Type, RHS> && IsEmptyUid<UID>
    inline UseWithPlaintype_<RHS> operator-(const RHS &rhs) const
    {
        return UseWithPlaintype_<RHS>{
            ratio_value_add<Type, Ratio, RHS, std::ratio<1>>(value, -rhs)};
    }

    /** @brief Unary negation */
    inline ThisType operator-() const
    {
        return -1 * (*this);
    }

    /**
     * Proxy compound assignment operators: +=, -=, *=, /=
     */

    /** @brief Multiplication assignment */
    template <typename T>
        requires requires(ThisType a, T b) { a = a * b; }
    inline ThisType &operator*=(const T &rhs)
    {
        value = ThisType{(*this) * rhs}.value;
        return *this;
    }

    /** @brief Division assignment */
    template <typename T>
        requires requires(ThisType a, T b) { a = a / b; }
    inline ThisType &operator/=(const T &rhs)
    {
        value = ThisType{*this / rhs}.value;
        return *this;
    }

    /** @brief Addition assignment */
    template <typename T>
        requires requires(ThisType a, T b) { a = a + b; }
    inline ThisType &operator+=(const T &rhs)
    {
        value = ThisType{*this + rhs}.value;
        return *this;
    }

    /** @brief Subtraction assignment */
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

    /**
     * Conversion operators
     */

    operator Type() const
        requires IsEmptyUid<UID>
    {
        return GetRealValue();
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

/**
 * Left side comparison
 */

/** @brief Left-compare with plain type for EmptyUnits */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires requires(Unit<RHS_Type, RHS_UID, RHS_Ratio> a, LHS b) {
        requires IsEmptyUid<RHS_UID>;
        { a.operator==(b) } -> std::convertible_to<bool>;
    }
inline bool operator==(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
{
    return rhs.operator==(lhs);
}

/**
 * Left side plaintype arithmetic
 */

/** @brief Left-multiply by plain type */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires CanOpMultiply<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS>
inline OpMultiplyType<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS> operator*(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
{
    return rhs.operator*(lhs);
}

/** @brief Left-divide with plain type */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires requires(LHS a, UnitExpI<Unit<RHS_Type, RHS_UID, RHS_Ratio>, -1> b) { b.operator*(a); }
inline OpMultiplyType<UnitExpI<Unit<RHS_Type, RHS_UID, RHS_Ratio>, -1>, LHS> operator/(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
{
    UnitExpI<Unit<RHS_Type, RHS_UID, RHS_Ratio>, -1> rhs_inv{1 / rhs.GetValue()};
    return rhs_inv.operator*(lhs);
}

/** @brief Left-add with plain type */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires CanOpAdd<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS>
inline OpAddType<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS> operator+(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
{
    return rhs.operator+(lhs);
}

/** @brief Left-subtract with plain type */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires CanOpAdd<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS>
inline OpAddType<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS> operator-(LHS lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
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