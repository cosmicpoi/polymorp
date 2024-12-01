#pragma once

#include <type_traits>
#include <ratio>
#include "_Consts.h"
#include "UnitIdentifier.h"
#include "TypeUtils.h"

//------------------------------------------------------------------------------
// Helpers for difficult ratio math
//------------------------------------------------------------------------------

template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
struct RatioEqualityHelper
{
    static constexpr intmax_t fac1 = LHS_Ratio::num * RHS_Ratio::den;
    static constexpr intmax_t fac2 = RHS_Ratio::num * LHS_Ratio::den;
};

template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires IsArithmetic<LHS_Type> && IsArithmetic<RHS_Type>
struct RatioEqualityHelper<LHS_Type, LHS_Ratio, RHS_Type, RHS_Ratio>
{
    static constexpr intmax_t fac1 = LHS_Ratio::num * RHS_Ratio::den;
    static constexpr intmax_t fac2 = RHS_Ratio::num * LHS_Ratio::den;
    using CommonType = std::common_type_t<LHS_Type, RHS_Type>;
    static constexpr intmax_t facMax = fac1 > fac2 ? fac1 : fac2;

    static constexpr CommonType eps1 = std::numeric_limits<LHS_Type>::epsilon();
    static constexpr CommonType eps2 = std::numeric_limits<RHS_Type>::epsilon();
    static constexpr CommonType eps = eps1 > eps2 ? eps1 : eps2;

    static constexpr CommonType epsilon = eps * facMax * EPS_TOLERANCE;
};

/**
 * Epsilon-based equality for IsArithmetic types.
 */
template <typename Type, IsRatio Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires(IsArithmetic<Type> &&
             IsArithmetic<RHS_Type> &&
             (RHS_Ratio::num > 0 && Ratio::num > 0))
constexpr bool typed_ratio_equality(Type value, RHS_Type rhs)
{
    // It might be tempting to add a check for if constexpr (std::is_same_v<Ratio, RHS_Ratio>), but
    // we want to override the default eps tolerance even for plain types (since they may have converted from Kilo)
    using Helper = RatioEqualityHelper<Type, Ratio, RHS_Type, RHS_Ratio>;
    using CommonType = typename Helper::CommonType;
    if constexpr (std::is_integral_v<Type> && std::is_integral_v<RHS_Type>)
    {
        return static_cast<CommonType>(value * Helper::fac1) == static_cast<CommonType>(rhs * Helper::fac2);
    }
    else
    {
        return std::abs(static_cast<CommonType>(value * Helper::fac1) - static_cast<CommonType>(rhs * Helper::fac2)) < Helper::epsilon;
    }
}

/**
 * Ratio addition/subtraction
 */

template <typename A, typename B>
concept CanRatioAdd = requires(std::common_type_t<A, B> x, intmax_t r) {
    IsRatioCompatible<std::common_type_t<A, B>>;
    { x *r } -> std::same_as<std::common_type_t<A, B>>;
    { x / r } -> std::same_as<std::common_type_t<A, B>>;
    { x + x } -> std::same_as<std::common_type_t<A, B>>;
};

template <typename A, typename B>
concept CanRatioSubtract = requires(std::common_type_t<A, B> x, intmax_t r) {
    IsRatioCompatible<std::common_type_t<A, B>>;
    { x *r } -> std::same_as<std::common_type_t<A, B>>;
    { x / r } -> std::same_as<std::common_type_t<A, B>>;
    { x - x } -> std::same_as<std::common_type_t<A, B>>;
};

/**
 * @brief Ratio addition/subtraction helper.
 * Supports expressions of the form `V1 * (N1/D1) + V2 * (N2/D2)`.
 */
template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires CanRatioAdd<LHS_Type, RHS_Type>
std::common_type_t<LHS_Type, RHS_Type> ratio_value_add(const LHS_Type &lhs, const RHS_Type &rhs)
{
    using ResType = std::common_type_t<LHS_Type, RHS_Type>;
    using LhsFac = typename CombineRatio<LHS_Ratio, RHS_Ratio>::lhsFac;
    using RhsFac = typename CombineRatio<LHS_Ratio, RHS_Ratio>::rhsFac;
    return MultiplyByRatio<LhsFac, ResType>(lhs) + MultiplyByRatio<RhsFac, ResType>(rhs);
}

template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires CanRatioSubtract<LHS_Type, RHS_Type>
std::common_type_t<LHS_Type, RHS_Type> ratio_value_subtract(const LHS_Type &lhs, const RHS_Type &rhs)
{
    using ResType = std::common_type_t<LHS_Type, RHS_Type>;
    using LhsFac = typename CombineRatio<LHS_Ratio, RHS_Ratio>::lhsFac;
    using RhsFac = typename CombineRatio<LHS_Ratio, RHS_Ratio>::rhsFac;
    return MultiplyByRatio<LhsFac, ResType>(lhs) - MultiplyByRatio<RhsFac, ResType>(rhs);
}

//------------------------------------------------------------------------------
// Unit class definition
//------------------------------------------------------------------------------

template <typename Type, UnitIdentifier UID, IsRatio Ratio>
struct IsValidUnit
{
    static constexpr bool value = false;
};

template <typename Type, UnitIdentifier UID, IsRatio Ratio>
    requires(IsRatioCompatible<Type> && Ratio::num > 0) ||
            (!IsRatioCompatible<Type> && std::is_same_v<Ratio, std::ratio<1>>)
struct IsValidUnit<Type, UID, Ratio>
{
    static constexpr bool value = true;
};

/**
 * @brief Unit definition
 * For each unit, we need to consider a few things:
 * - Is this an empty unit
 * - Is the underlying type a `IsArithmetic` type?
 * - Are ratios well-defined on this type? (Mult and div by intmax_t)
 * The behavior for each operator will vary based on these properties.
 */
template <typename Type, UnitIdentifier UID = EmptyUid, IsRatio Ratio = std::ratio<1>>
    requires(IsValidUnit<Type, UID, Ratio>::value)
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
     *  Internal helpers for arithmetic
     */

    // Used for multiplication
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    using ThisUnitMultiply_ = Unit<
        MultiplyType<Type, RHS_Type>,
        UIMult<UID, RHS_UID>,
        std::ratio_multiply<Ratio, RHS_Ratio>>;

    // Used for division
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    using ThisUnitDivide_ = Unit<
        DivideType<Type, RHS_Type>,
        UIDivide<UID, RHS_UID>,
        std::ratio_divide<Ratio, RHS_Ratio>>;

    // Used for adding/subtracting
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID> && CanAdd<Type, RHS_Type>
    using ThisUnitAdd_ = Unit<
        AddType<Type, RHS_Type>,
        UID,
        typename CombineRatio<Ratio, RHS_Ratio>::combinedRatio>;

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID> && CanSubtract<Type, RHS_Type>
    using ThisUnitSubtract_ = Unit<
        SubtractType<Type, RHS_Type>,
        UID,
        typename CombineRatio<Ratio, RHS_Ratio>::combinedRatio>;

    /**
     *  Multiplication and Division
     */

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
    inline Unit<MultiplyType<Type, RHS>, UID, Ratio> operator*(const RHS &rhs) const
    {
        return Unit<MultiplyType<Type, RHS>, UID, Ratio>{value * rhs};
    }

    /** @brief Divide by another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanDivide<Type, RHS_Type>
    inline ThisUnitDivide_<RHS_Type, RHS_UID, RHS_Ratio> operator/(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitDivide_<RHS_Type, RHS_UID, RHS_Ratio>{value / rhs.GetValue()};
    }

    /** @brief Divide by unitless scalar. */
    template <std::convertible_to<Type> RHS>
        requires CanDivide<Type, RHS>
    inline Unit<DivideType<Type, RHS>, UID, Ratio> operator/(const RHS &rhs) const
    {
        return Unit<DivideType<Type, RHS>, UID, Ratio>{value / rhs};
    }

    /** @brief Unary negation */
    inline ThisType operator-() const
    {
        return -1 * (*this);
    }

    /**
     * Addition and subtraction
     *
     *    These can be surprisingly tricky because we need to properly handle
     *    ratios. The overall method is to preserve as much info as possible in
     *    the ratio and rely on copy assignment to coerce the ratio into the one
     *    the user actually wants. For instance:
     *      Kilometer v = Kilometer{1} + Meter{1}; // expect 1.001km, not 1001m
     */

    /** @brief Add with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioAdd<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio> operator+(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio>{
            ratio_value_add<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue())};
    }

    /** @brief Add with a unitless scalar, if we are empty */
    template <std::convertible_to<Type> RHS>
        requires CanAdd<Type, RHS> && IsEmptyUid<UID>
    inline Unit<AddType<Type, RHS>, UID, Ratio> operator+(const RHS &rhs) const
    {
        return Unit<AddType<Type, RHS>, UID, Ratio>{
            ratio_value_add<Type, Ratio, RHS, std::ratio<1>>(value, rhs)};
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanRatioSubtract<Type, RHS_Type> && std::is_same_v<UID, RHS_UID>
    inline ThisUnitSubtract_<RHS_Type, RHS_UID, RHS_Ratio> operator-(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitSubtract_<RHS_Type, RHS_UID, RHS_Ratio>{
            ratio_value_subtract<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue())};
    }

    /** @brief Subtract with a unitless scalar, if we are empty */
    template <std::convertible_to<Type> RHS>
        requires CanSubtract<Type, RHS> && IsEmptyUid<UID>
    inline Unit<SubtractType<Type, RHS>, UID, Ratio> operator-(const RHS &rhs) const
    {
        return Unit<SubtractType<Type, RHS>, UID, Ratio>{
            ratio_value_subtract<Type, Ratio, RHS, std::ratio<1>>(value, rhs)};
    }

    /**
     * Compound assignment operators: +=, -=, *=, /=
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

    /**
     * Comparison
     */

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires requires(RHS_Type A, Type B) {
            requires std::is_same_v<UID, RHS_UID>;
            { A <=> B };
        }
    inline auto operator<=>(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        constexpr auto fac1 = Ratio::num * RHS_Ratio::den;
        constexpr auto fac2 = RHS_Ratio::num * Ratio::den;

        Type val1 = (value * fac1);
        RHS_Type val2 = (rhs.GetValue() * fac2);

        return val1 <=> val2;
    }

    /**
     * Equality operators. These are trickier than they look. In order for two
     * units A and B to be comparable:
     * - Their UIDs must match, or one is a plaintype and the other is an empty unit
     * And also, one of:
     * Builtin types:
     *   - Both types are ratio compatible, and epsilon is well-defined (builtin only)
     *   - Both types are ratio compatible, and are integral (builtin only)
     * User-defined types:
     *   - Underlying equality A == B must be defined
     * - Both sides are ratio-compatible (and ratios can be anything)
     * - Or, only one side is ratio-compatible (and it can have any ratio, but
     *   the other side must be 1/1)
     * - Or, neither side is ratio-compatible, and both have ratio 1/1
     */

    /**
     * @brief Compare with another unit of the same UID using `typed_ratio_equality`,
     * if both types are `IsArithmetic` types. Looks for a common epsilon under
     * the hood and multiplies the uncertainty by ratios.
     * - UIDs must match.
     * - Ratios must be well
     */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires((IsArithmetic<RHS_Type> && IsArithmetic<Type>) &&
                 requires(RHS_Type a, Type b) {
                     requires std::is_same_v<UID, RHS_UID>;
                     { a == b } -> std::convertible_to<bool>;
                 })
    inline bool operator==(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return typed_ratio_equality<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue());
    }

    /** @brief Comparison for std::is_arithmetic types for empty units */
    template <typename T>
        requires((IsArithmetic<T> && IsArithmetic<Type>) &&
                 requires(Type a, T b) {
                     requires IsEmptyUid<UID>;
                     { a == b } -> std::convertible_to<bool>;
                 })
    inline bool operator==(const T &rhs) const
    {
        return typed_ratio_equality<Type, Ratio, T, std::ratio<1>>(value, rhs);
    }

    /**
     * @brief Comparison between non-`std::arithmetic` types of the same UID and type,
     * provided that `==` is defined between them, and provided ratios are well-defined.
     * Use underlying `==`.
     */

    // // Compare with empty unit
    // template <typename T>
    //     requires((!std::is_arithmetic_v<T> || !std::is_arithmetic_v<T>) &&
    //              requires(Type a, T b) {
    //                  requires IsEmptyUid<UID>;
    //                  { a == b } -> std::convertible_to<bool>;
    //              })
    // inline bool operator==(const T &rhs) const
    // {
    //     return typed_ratio_equality<Type, Ratio, T, std::ratio<1>>(value, rhs);
    // }

    /** @brief Comparison between non-`std::arithmetic` types for empty units */

    /**
     * Conversion operators
     */

    /** @brief Allow conversion of this operator to its underlying type, if it's empty */
    operator Type() const
        requires IsEmptyUid<UID>
    {
        return GetRealValue();
    }

private:
    /** @brief Underlying value */
    Type value = 0;
};

/**
 * IsUnit concept
 */
template <typename T>
struct IsUnitHelper : std::false_type
{
};

template <typename Type, UnitIdentifier UID, IsRatio Ratio>
    requires IsValidUnit<Type, UID, Ratio>::value
struct IsUnitHelper<Unit<Type, UID, Ratio>> : std::true_type
{
};

/** @brief Concept to check if a type is a unit */
template <typename T>
concept IsUnit = IsUnitHelper<T>::value;

//------------------------------------------------------------------------------
// Type transformers
//------------------------------------------------------------------------------

/** @brief Check if unit U can actually be exponentiated by ratio Exp */
template <typename U, typename Exp>
concept UnitExpableRatio = IsUnit<U> && IsRatio<Exp> && RatioCanExp<typename U::ratio, Exp>;

/** @brief Get the resultant type of raising a unit to a rational exponent */
template <IsUnit U, IsRatio Exp>
    requires UnitExpableRatio<U, Exp>
using UnitExp = Unit<
    typename U::type,
    UIExp<typename U::uid, Exp>,
    typename RatioExp<typename U::ratio, Exp>::value>;

/** @brief Get the resultant type of raising a unit to an int exponent */
template <IsUnit U, intmax_t Exp>
using UnitExpI = UnitExp<U, std::ratio<Exp>>;

/** @brief Multiply an existing unit by a ratio */
template <IsUnit U, IsRatio Ratio>
using UnitMultRatio = Unit<
    typename U::type,
    typename U::uid,
    std::ratio_multiply<Ratio, typename U::ratio>>;

/** @brief Shorthand for empty unit */
template <typename T>
    requires std::is_arithmetic_v<T>
using EmptyUnit = Unit<T, EmptyUid>;

//------------------------------------------------------------------------------
// Left side plaintype operations
//------------------------------------------------------------------------------

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

/** @brief Left-multiply by plain type */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires CanOpMultiply<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS>
inline OpMultiplyType<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS> operator*(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs)
{
    return rhs.operator*(lhs);
}

/**
 * @brief Left-divide with plain type.
 * This is tricky because in order to define this properly we need a notion of
 * multiplicative inverse. Because of this, we choose to implement this only for
 * `std::is_arithmetic` types by default.
 */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires requires(LHS a, RHS_Type b) {
        // Only definable on is_arithmetic types
        requires std::is_arithmetic_v<LHS>;
        // Right-multiply is defineable for the inverse
        requires CanOpMultiply<UnitExpI<Unit<RHS_Type, RHS_UID, RHS_Ratio>, -1>, LHS>;
        // Reciprocals are well-defined on the underlying type
        { 1 / b } -> std::convertible_to<RHS_Type>;
    }
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
    requires requires(RHS_Type a) {
        { -1 * a };
    } && CanOpAdd<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS>
inline OpAddType<Unit<RHS_Type, RHS_UID, RHS_Ratio>, LHS> operator-(LHS lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
{
    return (-1 * rhs).operator+(lhs);
}

//------------------------------------------------------------------------------
// Shorthands for defining units
//------------------------------------------------------------------------------

template <typename T, StringLiteral Symbol>
    requires std::is_arithmetic_v<T>
using TypeAtomic = Unit<T, MakeUnitIdentifier<UnitAtomic<Symbol>>>;

template <StringLiteral Symbol>
using dAtomic = TypeAtomic<double, Symbol>;

template <StringLiteral Symbol>
using fAtomic = TypeAtomic<float, Symbol>;

template <StringLiteral Symbol>
using iAtomic = TypeAtomic<int, Symbol>;