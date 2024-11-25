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
concept SameUidAndType_ = requires {
    requires UnitLike<A>;
    requires UnitLike<B>;
    requires std::is_same_v<typename A::uid, typename B::uid>;
    requires std::is_same_v<typename A::type, typename B::type>;
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
    void Print(std::ostream &os = std::cout) const
    {
        os << "u{ ";
        PrintInfo(os);
        os << " " << value << " }";
    }

    /** @brief Logging shorthand to print a header, contents, and newline */
    void Log(std::ostream &os) const
    {
        os << "Unit print" << std::endl;
        Print(os);
        os << std::endl;
    }

    /** @brief Constructor for converting from literal */
    template <typename T>
        requires std::is_convertible_v<T, Type>
    Unit(T val)
        : value(val){};

    /**
     * Copy assignments
     */
    template <UnitLike RHS>
    Unit<Type, UID, Ratio> &operator=(RHS rhs)
        requires SameUid_<ThisType, RHS>
    {
        Type product = RatioMult<Type, typename RHS::ratio>(value * rhs.value);
        value = RatioDiv<Type, Ratio>(product);
        return *this;
    }

    /**
     *  Multiplication and Division
     */

    /** @brief Multiply with another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    auto operator*(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return Unit<ResType, UIMult<UID, RHS_UID>, std::ratio_multiply<Ratio, RHS_Ratio>>{value * rhs.value};
    }

    /** @brief Multiply with unitless scalar. */
    template <typename RHS>
        requires std::is_convertible_v<RHS, Type>
    auto operator*(RHS rhs)
    {
        using ResType = std::common_type_t<Type, RHS>;
        return operator*(Unit<ResType, EmptyUid>{rhs});
    }

    /** @brief Divide by another unit. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    inline auto operator/(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return Unit<ResType, UIDivide<UID, RHS_UID>, std::ratio_divide<Ratio, RHS_Ratio>>{value / rhs.value};
    }

    /** @brief Divide by unitless scalar. */
    inline auto operator/(Type rhs)
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
    auto operator+(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
        requires(std::is_same_v<UID, RHS_UID>)
    {
        using ResType = std::common_type_t<Type, RHS_Type>;
        return Unit<ResType, UID, CombineRatio<Ratio, RHS_Ratio>>{
            (value / (Ratio::den * RHS_Ratio::num)) + (rhs.value / (RHS_Ratio::den * Ratio::num)) //
        };
    }

    /** @brief Subtract with another unit, only if UIDs match. Follow default language promotion rules */
    template <typename RHS_Type, UnitIdentifier RHS_UID, IsRatio RHS_Ratio>
    auto operator-(Unit<RHS_Type, RHS_UID, RHS_Ratio> rhs)
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

    // auto operator<=> const {

    // }

    // bool operator==(const Point& other) const {
    //     return this->x == other.x && this->y == other.y;
    // }

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

// Other operator overloads
template <typename Type, UnitIdentifier UID, IsRatio Ratio>
std::ostream &operator<<(std::ostream &os, Unit<Type, UID, Ratio> val)
{
    return val.operator<<(os);
}

/** @brief Multiply with another unit. Follow default language promotion rules */
template <typename LHS, IsUnit Unit_RHS>
auto operator*(LHS lhs, Unit_RHS rhs)
{
    return rhs * lhs;
}

/** @brief Divide with another unit. Follow default language promotion rules */ // template <typename LHS, IsUnit Unit_RHS>
template <typename LHS, IsUnit Unit_RHS>
auto operator/(LHS lhs, Unit_RHS rhs)
{
    return lhs * unit_pow<std::ratio<-1>>(rhs);
}

// Stronger versions of underscored aliases above to check for Unit. We have to do it this way because of declaration order
template <typename A, typename B>
concept SameUid = IsUnit<A> && IsUnit<B> && SameUid_<A, B>;

template <typename A, typename B>
concept SameUidAndType = IsUnit<A> && IsUnit<B> && SameUidAndType_<A, B>;

// Get the resulant unit from multiplying two units. Left-side type always dominates.
template <IsUnit A, IsUnit B>
using UnitMult = Unit<typename A::type, UIMult<typename A::uid, typename B::uid>>;