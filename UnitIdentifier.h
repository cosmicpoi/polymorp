#pragma once
#include "StringLiteral.h"
#include "TypeUtils.h"
#include <string>

/**
 * UnitIdentifier - the unique representation of a particular unit. Comprised of atomic units and exponents
 * For instance: (meter, 1), (second, -2) -> acceleration (m/s^2)
 * These are constructed as a list of tuples of StringLiterals and ints
 */

template <StringLiteral Symbol, int Exponent>
struct UnitLeaf
{
    static constexpr StringLiteral symbol = Symbol;
    static constexpr int exponent = Exponent;

    static void Print()
    {
        PrintStrLit<Symbol>();
        std::cout << " " << Exponent;
    }
};

/** Concept to match UnitLeaf */

// Define the primary template
template <typename T>
struct IsUnitLeafHelper : std::false_type
{
};

// Specialization for the `Unit` template
template <StringLiteral Symbol, int Exponent>
struct IsUnitLeafHelper<UnitLeaf<Symbol, Exponent>> : std::true_type
{
};

// Shorthand
template <typename T>
concept IsUnitLeaf = IsUnitLeafHelper<T>::value && HasPrint<T>;

/** Basic definition of UnitLeafVector */

template <typename... Ts>
    requires(IsUnitLeaf<Ts> && ...)
struct UnitLeafVector
{
    static void Print()
    {
        ([&]
         {
            Ts::Print();
            std::cout << "; "; }(), ...);
        std::cout << std::endl;
    }
};

/** Concept to match UnitLeafVector */

// Define the primary template

template <typename T>
struct IsUnitLeafVectorHelper : std::false_type
{
};

// // Specialization for the `Unit` template
template <typename... Ts>
    requires(IsUnitLeaf<Ts> && ...)
struct IsUnitLeafVectorHelper<UnitLeafVector<Ts...>> : std::true_type
{
};

// Shorthand
template <typename T>
concept IsUnitLeafVector = IsUnitLeafVectorHelper<T>::value && HasPrint<T>;

/** Prepend to a UnitLeafVector */

template <typename H, typename V>
    requires(IsUnitLeaf<H> and IsUnitLeafVector<V>)
struct ULPrependHelper;

template <IsUnitLeaf H, IsUnitLeaf... T>
struct ULPrependHelper<H, UnitLeafVector<T...>>
{
    using type = UnitLeafVector<H, T...>;
};

template <typename H, typename V>
    requires(IsUnitLeaf<H> && IsUnitLeafVector<V>)
using ULPrepend = typename ULPrependHelper<H, V>::type;

/** Append to a UnitLeafVector */

template <typename H, typename V>
    requires(IsUnitLeaf<H> && IsUnitLeafVector<V>)
struct ULAppendHelper;

template <IsUnitLeaf H, IsUnitLeaf... T>
struct ULAppendHelper<H, UnitLeafVector<T...>>
{
    using type = UnitLeafVector<T..., H>;
};

template <typename H, typename V>
    requires(IsUnitLeaf<H> && IsUnitLeafVector<V>)
using ULAppend = typename ULAppendHelper<H, V>::type;

/* recursively - for the exercise */

// template<typename H, typename V> requires (IsUnitLeaf<H> and IsUnitLeafVector<V>)
// struct ULAppend_;

// template<IsUnitLeaf E, IsUnitLeaf H, IsUnitLeaf... T>
// struct ULAppend_<E, UnitLeafVector<H, T...>>
// {
//     using type = ULPrepend<H, typename ULAppend_<E, UnitLeafVector<T...>>::type>;
// };

// template<IsUnitLeaf E, IsUnitLeaf... T> requires (sizeof...(T) == 0)
// struct ULAppend_<E, UnitLeafVector<T...>>
// {
//     using type = UnitLeafVector<E>;
// };

/* popback - for the exercise */

// template<typename T> requires IsUnitLeafVector<T>
// struct PopBack;

// template<IsUnitLeaf H, IsUnitLeaf... T>
// struct PopBack<UnitLeafVector<H, T...>>
// {
//     using type = UnitLeafVector<T...>;
// };

/** Remove first occurrence of element R from Vector V */

template <typename R, typename V>
    requires(IsUnitLeaf<R> && IsUnitLeafVector<V>)
struct ULRemoveFirst_;

template <IsUnitLeaf R, IsUnitLeaf... T>
struct ULRemoveFirst_<R, UnitLeafVector<R, T...>> // head is matching
{
    using type = UnitLeafVector<T...>;
};

template <IsUnitLeaf R, IsUnitLeaf H, IsUnitLeaf... T>
struct ULRemoveFirst_<R, UnitLeafVector<H, T...>> // head is not matching
{
    using type = ULPrepend<H, typename ULRemoveFirst_<R, UnitLeafVector<T...>>::type>;
};

template <IsUnitLeaf R, IsUnitLeaf... T>
    requires(sizeof...(T) == 0)
struct ULRemoveFirst_<R, UnitLeafVector<T...>> // handle empty case
{
    using type = UnitLeafVector<T...>;
};

// alias
template <typename R, typename V>
    requires(IsUnitLeaf<R> && IsUnitLeafVector<V>)
using ULRemoveFirst = typename ULRemoveFirst_<R, V>::type;

/** Comparison function */
template <typename A, typename B>
concept _CompareExp = requires {
    { A::exponent } -> std::convertible_to<int>;
    { B::exponent } -> std::convertible_to<int>;
    A::exponent > B::exponent;
} && (A::exponent < B::exponent);

template <typename A, typename B>
concept _CompareSymb = requires {
    A::symbol;
    B::symbol;
} && (CompareStrings<A::symbol, B::symbol>::value);

template <typename A, typename B>
concept ULCompare = IsUnitLeaf<A> && IsUnitLeaf<B> && _CompareSymb<A, B>;
// concept ULCompare = IsUnitLeaf<A> && IsUnitLeaf<B> && _CompareExp<A, B>;

// Comparison selector GreaterOf
template <typename A, typename B>
    requires(IsUnitLeaf<A> && IsUnitLeaf<B>)
struct ULSmallerOf_
{
};

template <typename A, typename B>
    requires(IsUnitLeaf<A> && IsUnitLeaf<B> && ULCompare<A, B>)
struct ULSmallerOf_<A, B>
{
    using type = A;
};

template <typename A, typename B>
    requires(IsUnitLeaf<A> && IsUnitLeaf<B> && !ULCompare<A, B>)
struct ULSmallerOf_<A, B>
{
    using type = B;
};

template <typename A, typename B>
    requires(IsUnitLeaf<A> && IsUnitLeaf<B>)
using ULSmallerOf = typename ULSmallerOf_<A, B>::type;

/** Define min */
template <typename R, typename V>
    requires(IsUnitLeaf<R> && IsUnitLeafVector<V>)
struct ULMinRecurse;

template <IsUnitLeaf M, IsUnitLeaf H, IsUnitLeaf... T>
struct ULMinRecurse<M, UnitLeafVector<H, T...>>
{
    using _Recur = ULMinRecurse<ULSmallerOf<M, H>, UnitLeafVector<T...>>;
    using min = typename _Recur::min;
};

template <IsUnitLeaf M, IsUnitLeaf... T>
struct ULMinRecurse<M, UnitLeafVector<T...>>
{
    using min = M;
};

template <typename V>
    requires(IsUnitLeafVector<V>)
struct ULMin_;

template <IsUnitLeaf H, IsUnitLeaf... T>
struct ULMin_<UnitLeafVector<H, T...>>
{
    using min = typename ULMinRecurse<H, UnitLeafVector<T...>>::min;
};

template <typename V>
    requires(IsUnitLeafVector<V>)
using ULMin = typename ULMin_<V>::min;