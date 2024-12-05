#pragma once

#include <type_traits>
#include <ratio>
#include "_Consts.h"
#include "UnitIdentifier.h"
#include "TypeUtils.h"

//------------------------------------------------------------------------------
// Helpers for difficult ratio math
//------------------------------------------------------------------------------

// User-defined version
template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
struct RatioEqualityHelper
{
    static constexpr intmax_t fac1 = LHS_Ratio::num * RHS_Ratio::den;
    static constexpr intmax_t fac2 = RHS_Ratio::num * LHS_Ratio::den;
};

// Builtin IsArithmetic version
template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires IsArithmetic<LHS_Type> && IsArithmetic<RHS_Type>
struct RatioEqualityHelper<LHS_Type, LHS_Ratio, RHS_Type, RHS_Ratio>
{
    static constexpr intmax_t fac1 = LHS_Ratio::num * RHS_Ratio::den;
    static constexpr intmax_t fac2 = RHS_Ratio::num * LHS_Ratio::den;
    using Common = CommonType<LHS_Type, RHS_Type>;
    static constexpr intmax_t facMax = fac1 > fac2 ? fac1 : fac2;

    static constexpr Common eps1 = std::numeric_limits<LHS_Type>::epsilon();
    static constexpr Common eps2 = std::numeric_limits<RHS_Type>::epsilon();
    static constexpr Common eps = eps1 > eps2 ? eps1 : eps2;

    static constexpr Common epsilon = eps * facMax * EPS_TOLERANCE;
};

/**
 * Equality helper
 */

/**
 * Equality Helper.
 *
 * These is trickeir than it looks. In order for two units A and B to be comparable:
 * - Their UIDs must match, or one is a plaintype and the other is an empty unit
 * And also, one of:
 * Builtin types:
 *   - Both types are ratio compatible, and epsilon is well-defined (builtin only) -> cross-multiply with epsilon check
 *   - Both types are ratio compatible, and are integral (builtin only)            -> cross-multiply with direct equality
 * User-defined types:
 * - Underlying equality A == B must be defined. In addition, either:
 *   - Both sides are ratio-compatible (and ratios can be anything)                -> cross-multiply with direct equality
 *   - Or, only one side is ratio-compatible (and it can have any ratio, but       -> compare real values
 *     the other side must be 1/1)
 *   - Or, neither side is ratio-compatible, and both have ratio 1/1
 */

/** @brief Cross-multiply-based equality for builtin types */
template <typename Type, IsRatio Ratio, typename RHS_Type, IsRatio RHS_Ratio>
    requires(RHS_Ratio::num > 0 && Ratio::num > 0)
constexpr bool typed_ratio_equality(const Type &value, const RHS_Type &rhs)
{
    // It might be tempting to add a check for if constexpr (std::is_same_v<Ratio, RHS_Ratio>), but
    // we want to override the default eps tolerance even for plain types (since they may have converted from Kilo)
    using Helper = RatioEqualityHelper<Type, Ratio, RHS_Type, RHS_Ratio>;
    if constexpr (IsIntegral<Type, RHS_Type>)
    {
        using Common = typename Helper::Common;
        return static_cast<Common>(value * Helper::fac1) == static_cast<Common>(rhs * Helper::fac2);
    }
    // Fallback for IsIntegral<Type, RHS_Type> builtins AND for user-defined types
    else if constexpr (IsArithmetic<Type, RHS_Type>)
    {
        using Common = typename Helper::Common;
        return std::abs(static_cast<Common>(value * Helper::fac1) - static_cast<Common>(rhs * Helper::fac2)) < Helper::epsilon;
    }

    else if constexpr (IsRatioCompatible<Type, RHS_Type>)
    {
        // Don't need to check that Type * intmax_t is valid because this is enforced by IsRatioCompatible_
        static_assert((requires(Type a, RHS_Type b, intmax_t c) { {a * c}; {b * c}; }));
        return ConvertOrConstruct<Type>(value * Helper::fac1) == ConvertOrConstruct<RHS_Type>(rhs * Helper::fac2);
    }
    else
    {
        return MultiplyByRatio<Ratio, Type>(value) == MultiplyByRatio<RHS_Ratio, RHS_Type>(rhs);
    }
}

/**
 * Ratio addition/subtraction
 *    These can be surprisingly tricky because we need to properly handle
 *    ratios. The overall method is to preserve as much info as possible in
 *    the ratio and rely on copy assignment to coerce the ratio into the one
 *    the user actually wants. For instance:
 *      Kilometer v = Kilometer{1} + Meter{1}; // expect 1.001km, not 1001m
 *
 * In order for units A and B to be addable:
 * - Their UIDs must match, or one is a plaintype and the other is an empty unit
 * - Addition A+B is well-defined
 * And also, one of:
 * - AddType<A,B> is ratio-compatible              -> add with common ratio
 * - AddType<A,B> is not ratio-compatible          -> add with GetRealValue() and return ratio 1
 *   (A, B can be either ratio-compatible or not)
 *
 * Vice versa for subtraction (A-B is well defined, SubtractType<A, B> is ratio-compatible)
 * Note that we use AddType/SubtractType for type promotion instead of CommonType
 * so that we can imitate the underlying behavior more closely, especially for user-defined
 * types. We assume that the semantics of A+B promote to the preferred type
 * (which potentially is neither A or B).
 */

// Use AddType instead of CommonType
template <typename A, typename B>
concept CanRatioAdd = requires(AddType<A, B> x, intmax_t r) {
    requires CanAdd<A, B>;
    // Checks for closure of * and /
    requires IsRatioCompatible<AddType<A, B>>;
    // Check that the promoted type can convert to itself - the logic is that semantics
    // similar to A+B should return decltype(A+B), not a distinct decltype ((A+B)+(A+B))
    { x + x } -> std::convertible_to<AddType<A, B>>;
};

// Use SubtractType instead of CommonType
template <typename A, typename B>
concept CanRatioSubtract = requires(SubtractType<A, B> x, intmax_t r) {
    requires CanSubtract<A, B>;
    // Checks for closure of * and /
    requires IsRatioCompatible<SubtractType<A, B>>;
    // Check that the promoted type can convert to itself - the logic is that semantics
    // similar to A+B should return decltype(A-B), not a distinct decltype ((A-B)-(A-B))
    { x - x } -> std::convertible_to<SubtractType<A, B>>;
};

template <typename LHS_Type, UnitIdentifier LHS_UID, IsRatio LHS_Ratio, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires(std::is_same_v<LHS_UID, RHS_UID> &&
             CanAdd<LHS_Type, RHS_Type>)
struct UnitRatioAdd
{
    using Type = AddType<LHS_Type, RHS_Type>;
    using UID = LHS_UID;
    using Ratio = typename TypedRatioAddHelper<AddType<LHS_Type, RHS_Type>, LHS_Ratio, RHS_Ratio>::combinedRatio;
};

template <typename LHS_Type, UnitIdentifier LHS_UID, IsRatio LHS_Ratio, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires(std::is_same_v<LHS_UID, RHS_UID> &&
             CanSubtract<LHS_Type, RHS_Type>)
struct UnitRatioSubtract
{
    using Type = SubtractType<LHS_Type, RHS_Type>;
    using UID = LHS_UID;
    using Ratio = typename TypedRatioAddHelper<SubtractType<LHS_Type, RHS_Type>, LHS_Ratio, RHS_Ratio>::combinedRatio;
};

/**
 * @brief Ratio addition/subtraction helper.
 * Supports expressions of the form `V1 * (N1/D1) + V2 * (N2/D2)`.
 */
template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
AddType<LHS_Type, RHS_Type> ratio_value_add(const LHS_Type &lhs, const RHS_Type &rhs)
{
    if constexpr (CanRatioAdd<LHS_Type, RHS_Type>)
    {
        using LhsFac = typename RatioAddHelper<LHS_Ratio, RHS_Ratio>::lhsFac;
        using RhsFac = typename RatioAddHelper<LHS_Ratio, RHS_Ratio>::rhsFac;
        return AddType<LHS_Type, RHS_Type>{
            MultiplyByRatio<LhsFac, AddType<LHS_Type, RHS_Type>>(lhs) +
            MultiplyByRatio<RhsFac, AddType<LHS_Type, RHS_Type>>(rhs)};
    }
    else
    {
        return AddType<LHS_Type, RHS_Type>{MultiplyByRatio<LHS_Ratio, LHS_Type>(lhs) + MultiplyByRatio<RHS_Ratio, RHS_Type>(rhs)};
    }
}

template <typename LHS_Type, IsRatio LHS_Ratio, typename RHS_Type, IsRatio RHS_Ratio>
SubtractType<LHS_Type, RHS_Type> ratio_value_subtract(const LHS_Type &lhs, const RHS_Type &rhs)
{
    if constexpr (CanRatioSubtract<LHS_Type, RHS_Type>)
    {
        using LhsFac = typename RatioAddHelper<LHS_Ratio, RHS_Ratio>::lhsFac;
        using RhsFac = typename RatioAddHelper<LHS_Ratio, RHS_Ratio>::rhsFac;
        return MultiplyByRatio<LhsFac, SubtractType<LHS_Type, RHS_Type>>(lhs) -
               MultiplyByRatio<RhsFac, SubtractType<LHS_Type, RHS_Type>>(rhs);
    }
    else
    {
        return SubtractType<LHS_Type, RHS_Type>{MultiplyByRatio<LHS_Ratio, LHS_Type>(lhs) - MultiplyByRatio<RHS_Ratio, RHS_Type>(rhs)};
    }
}

//------------------------------------------------------------------------------
// Unit class definition
//------------------------------------------------------------------------------

template <typename Type, UnitIdentifier UID, IsRatio Ratio>
struct IsValidUnit_
{
    static constexpr bool value = false;
};

template <typename Type, UnitIdentifier UID, IsRatio Ratio>
    requires(IsRatioCompatible<Type> && Ratio::num > 0) ||
            (!IsRatioCompatible<Type> && std::is_same_v<Ratio, std::ratio<1>>)
struct IsValidUnit_<Type, UID, Ratio>
{
    static constexpr bool value = true;
};

template <typename Type, typename UID, typename Ratio>
concept IsValidUnit = UnitIdentifier<UID> && IsRatio<Ratio> &&
                      IsValidUnit_<Type, UID, Ratio>::value;

/**
 * @brief Unit definition
 * For each unit, we need to consider a few things:
 * - Is this an empty unit? (Checked by IsEmptyUid<UID>)
 * - Is the underlying type a builtin type? (Checked by `IsArithmetic<Type>`)
 * - Are ratios well-defined on this type? (Mult and div by intmax_t)
 * The behavior for each operator will vary based on these properties.
 */
template <typename Type, UnitIdentifier UID = EmptyUid, IsRatio Ratio = std::ratio<1>>
    requires(IsValidUnit<Type, UID, Ratio>)
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
     * @brief Constructor from assignable or convertible rvalue
     * Note: need to check that ratio does not match to avoid overriding copy constructor
     */
    template <typename Other_Type, UnitIdentifier Other_UID, IsRatio Other_Ratio>
        requires(std::is_same_v<UID, Other_UID> &&
                 (!ConvertibleOrConstructible<Type, Other_Type>) &&
                 AssignableTo<Type, Other_Type> &&
                 (!std::is_same_v<Type, Other_Type>))
    inline Unit(const Unit<Other_Type, Other_UID, Other_Ratio> &&val)
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
        if constexpr (std::is_same_v<Ratio, std::ratio<1>>)
        {
            return value;
        }
        else
        {
            return MultiplyByRatio<Ratio, Type>(value);
        }
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
     *  Multiplication and Division
     */

    /* Divide/multiply type helpers */

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

    /** @brief Multiply with another unit. */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanMultiply<Type, RHS_Type>
    inline ThisUnitMultiply_<RHS_Type, RHS_UID, RHS_Ratio> operator*(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitMultiply_<RHS_Type, RHS_UID, RHS_Ratio>{value * rhs.GetValue()};
    }

    /** @brief Divide by another unit.  */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires CanDivide<Type, RHS_Type>
    inline ThisUnitDivide_<RHS_Type, RHS_UID, RHS_Ratio> operator/(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitDivide_<RHS_Type, RHS_UID, RHS_Ratio>{value / rhs.GetValue()};
    }

    /** @brief Unary negation */
    inline ThisType operator-() const
    {
        return -1 * (*this);
    }

    /**
     * Addition and subtraction
     */

    /** Add/subtract type helpers */

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID> && CanAdd<Type, RHS_Type>
    using ThisUnitAdd_ = Unit<
        typename UnitRatioAdd<Type, UID, Ratio, RHS_Type, RHS_UID, RHS_Ratio>::Type,
        typename UnitRatioAdd<Type, UID, Ratio, RHS_Type, RHS_UID, RHS_Ratio>::UID,
        typename UnitRatioAdd<Type, UID, Ratio, RHS_Type, RHS_UID, RHS_Ratio>::Ratio>;

    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID> && CanSubtract<Type, RHS_Type>
    using ThisUnitSubtract_ = Unit<
        typename UnitRatioSubtract<Type, UID, Ratio, RHS_Type, RHS_UID, RHS_Ratio>::Type,
        typename UnitRatioSubtract<Type, UID, Ratio, RHS_Type, RHS_UID, RHS_Ratio>::UID,
        typename UnitRatioSubtract<Type, UID, Ratio, RHS_Type, RHS_UID, RHS_Ratio>::Ratio>;

    /** @brief Add with another unit */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID> &&
                 CanAdd<Type, RHS_Type>
    inline ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio> operator+(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitAdd_<RHS_Type, RHS_UID, RHS_Ratio>{
            ratio_value_add<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue())};
    }

    /** @brief Subtract with another unit */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires std::is_same_v<UID, RHS_UID> &&
                 CanAdd<Type, RHS_Type>
    inline ThisUnitSubtract_<RHS_Type, RHS_UID, RHS_Ratio> operator-(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return ThisUnitSubtract_<RHS_Type, RHS_UID, RHS_Ratio>{
            ratio_value_subtract<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue())};
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
     * Equality Comparison for builtin types
     */

    /** @brief Comparison for builtin IsArithmetic types (integral and non-integral) */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
        requires(std::is_same_v<UID, RHS_UID> &&
                 IsEqualityComparable<Type, RHS_Type>)
    inline bool operator==(const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs) const
    {
        return typed_ratio_equality<Type, Ratio, RHS_Type, RHS_Ratio>(value, rhs.GetValue());
    }
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
    requires IsValidUnit<Type, UID, Ratio>
struct IsUnitHelper<Unit<Type, UID, Ratio>> : std::true_type
{
};

/** @brief Concept to check if a type is a unit */
template <typename T>
concept IsUnit = IsUnitHelper<std::decay_t<T>>::value;

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
    requires IsValidUnit<T, EmptyUid, std::ratio<1>>
using EmptyUnit = Unit<T, EmptyUid, std::ratio<1>>;

//------------------------------------------------------------------------------
// Plaintype operations
//------------------------------------------------------------------------------

/**
 * We generally prefer to implement these as non-members because they allow us to
 * explicitly check !IsUnit, avoiding recursion from e.g. `Empty{1} + KiloEmpty{1} == 1001`
 */

// Kind of hacky, but without these checks the Vector type will be treated like a scalar, so that
// Unit<Meter> * Vector2<Meter> -> Unit<Vector2<Meter^2>> instead of Vector2<Unit<Meter^2>>.
// Would like to replace this with a better solution.
template <typename T>
concept IsVectorLike = requires {
    { T::n } -> std::same_as<size_t>;
};

template <typename T>
concept IsMatrixLike = requires {
    { T::m } -> std::same_as<size_t>;
};

template <typename T>
concept IsUnitOrContainerLike = IsUnit<T> || IsContainer<T>;

/**
 * Right and left-side multiplication and division
 */

/** @brief Multiply with unitless scalar. */
template <typename Type, UnitIdentifier UID, IsRatio Ratio, typename RHS>
    requires((!IsUnitOrContainerLike<RHS>) &&
             CanMultiply<Type, RHS>)
inline Unit<MultiplyType<Type, RHS>, UID, Ratio> operator*(const Unit<Type, UID, Ratio> &lhs_unit, const RHS &rhs)
{
    return Unit<MultiplyType<Type, RHS>, UID, Ratio>{lhs_unit.GetValue() * rhs};
}

/** @brief Divide by unitless scalar. */
template <typename Type, UnitIdentifier UID, IsRatio Ratio, typename RHS>
    requires((!IsUnitOrContainerLike<RHS>) &&
             CanDivide<Type, RHS>)
inline Unit<DivideType<Type, RHS>, UID, Ratio> operator/(const Unit<Type, UID, Ratio> &lhs_unit, const RHS &rhs)
{
    return Unit<DivideType<Type, RHS>, UID, Ratio>{lhs_unit.GetValue() / rhs};
}

/** @brief Left-multiply by plain type */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires((!IsUnitOrContainerLike<LHS>) &&
             CanMultiply<LHS, RHS_Type>)
inline Unit<MultiplyType<LHS, RHS_Type>, RHS_UID, RHS_Ratio> operator*(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs_unit)
{
    return Unit<MultiplyType<LHS, RHS_Type>, RHS_UID, RHS_Ratio>{lhs * rhs_unit.GetValue()};
}

/** @brief Left-divide with plain type. */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires((!IsUnitOrContainerLike<LHS>) &&
             CanDivide<LHS, RHS_Type>)
inline Unit<DivideType<LHS, RHS_Type>, UIInvert<RHS_UID>, RatioInvert<RHS_Ratio>> operator/(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs_unit)
{
    return Unit<DivideType<LHS, RHS_Type>, UIInvert<RHS_UID>, RatioInvert<RHS_Ratio>>{lhs / rhs_unit.GetValue()};
}

/**
 * Right-side and left-side EmptyUnit addition and subtraction
 */

template <typename SharedType, IsRatio Ratio>
struct UnitAddSubtractRatio_
{
    using ratio = std::ratio<1>;
};

template <typename SharedType, IsRatio Ratio>
    requires IsRatioCompatible<SharedType>
struct UnitAddSubtractRatio_<SharedType, Ratio>
{
    using ratio = Ratio;
};

template <typename SharedType, IsRatio Ratio>
using UnitAddSubtractRatio = typename UnitAddSubtractRatio_<SharedType, Ratio>::ratio;

template <UnitIdentifier UID, IsRatio Ratio, typename SharedType>
using UnitAddSubtractType = Unit<SharedType, UID, UnitAddSubtractRatio<SharedType, Ratio>>;

/** @brief Add with another type, if resultant type is ratio-compatible */
template <typename Type, UnitIdentifier UID, IsRatio Ratio, typename RHS>
    requires((IsEmptyUid<UID> && !IsUnitOrContainerLike<RHS>) &&
             CanAdd<Type, RHS>)
inline UnitAddSubtractType<UID, Ratio, AddType<Type, RHS>> operator+(const Unit<Type, UID, Ratio> &lhs_unit, const RHS &rhs)
{
    return UnitAddSubtractType<UID, Ratio, AddType<Type, RHS>>{
        ratio_value_add<Type, Ratio, RHS, std::ratio<1>>(lhs_unit.GetValue(), rhs)};
}

/** @brief Left-side ratio-compatible add */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires((IsEmptyUid<RHS_UID> && !IsUnitOrContainerLike<LHS>) &&
             CanAdd<LHS, RHS_Type>)
inline UnitAddSubtractType<RHS_UID, RHS_Ratio, AddType<LHS, RHS_Type>> operator+(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs_unit)
{
    return UnitAddSubtractType<RHS_UID, RHS_Ratio, AddType<LHS, RHS_Type>>{
        ratio_value_add<LHS, std::ratio<1>, RHS_Type, RHS_Ratio>(lhs, rhs_unit.GetValue())};
}

/** @brief Subtract with another type, if resultant type is ratio-compatible */
template <typename Type, UnitIdentifier UID, IsRatio Ratio, typename RHS>
    requires((IsEmptyUid<UID> && !IsUnitOrContainerLike<RHS>) &&
             CanSubtract<Type, RHS>)
inline UnitAddSubtractType<UID, Ratio, SubtractType<Type, RHS>> operator-(const Unit<Type, UID, Ratio> &lhs_unit, const RHS &rhs)
{
    return UnitAddSubtractType<UID, Ratio, SubtractType<Type, RHS>>{
        ratio_value_subtract<Type, Ratio, RHS, std::ratio<1>>(lhs_unit.GetValue(), rhs)};
}

/** @brief Left-side ratio-compatible subtract */
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires((IsEmptyUid<RHS_UID> && !IsUnitOrContainerLike<LHS>) &&
             CanSubtract<LHS, RHS_Type>)
inline UnitAddSubtractType<RHS_UID, RHS_Ratio, SubtractType<LHS, RHS_Type>> operator-(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs_unit)
{
    return UnitAddSubtractType<RHS_UID, RHS_Ratio, SubtractType<LHS, RHS_Type>>{
        ratio_value_subtract<LHS, std::ratio<1>, RHS_Type, RHS_Ratio>(lhs, rhs_unit.GetValue())};
}

/**
 * Right-side and left-side empty unit comparison equality
 */

/** @brief Right-side comparison for builtin IsArithmetic types for empty units (integral and non-integral)*/
template <typename Type, UnitIdentifier UID, IsRatio Ratio, typename RHS>
    requires((IsEmptyUid<UID> && !IsUnitOrContainerLike<RHS>) &&
             IsEqualityComparable<Type, RHS>)
inline bool operator==(const Unit<Type, UID, Ratio> &lhs_unit, const RHS &rhs)
{
    return typed_ratio_equality<Type, Ratio, RHS, std::ratio<1>>(lhs_unit.GetValue(), rhs);
}

/** @brief Left-side comparison for builtin IsArithmetic types for empty units (integral and non-integral)*/
template <typename LHS, typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    requires((IsEmptyUid<RHS_UID> && !IsUnitOrContainerLike<LHS>) &&
             IsEqualityComparable<LHS, RHS_Type>)
inline bool operator==(const LHS &lhs, const Unit<RHS_Type, RHS_UID, RHS_Ratio> &rhs_unit)
{
    return typed_ratio_equality<LHS, std::ratio<1>, RHS_Type, RHS_Ratio>(lhs, rhs_unit.GetValue());
}

//------------------------------------------------------------------------------
// Type utils
//------------------------------------------------------------------------------

/**
 * Shorthands for defining units
 */

template <typename T, StringLiteral Symbol>
using TypeAtomic = Unit<T, UnitAtomic<Symbol>>;

template <StringLiteral Symbol>
using dAtomic = TypeAtomic<double, Symbol>;

template <StringLiteral Symbol>
using fAtomic = TypeAtomic<float, Symbol>;

template <StringLiteral Symbol>
using iAtomic = TypeAtomic<int, Symbol>;

/**
 * Concepts for matching unit symbols
 */

/** @brief Concept to check that a type is a unit with a particular UID */
template <typename T, typename UID>
concept IsUnitWithUID = requires {
    requires UnitIdentifier<UID>;
    requires IsUnit<T>;
    typename std::decay_t<T>::uid;
    requires std::is_same_v<typename std::decay_t<T>::uid, UID>;
};

/** @brief Concept to check that a type is a unit an atomic UID corresponding to this symbol */
template <typename T, StringLiteral Symbol>
concept IsUnitWithSymbol = IsUnitWithUID<T, UnitAtomic<Symbol>>;