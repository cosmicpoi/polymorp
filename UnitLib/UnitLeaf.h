
// ULSorting functionality adapted from https://www.slamecka.cz/posts/2021-03-17-cpp-metaprogramming-exercises-1/

#pragma once

#include <iostream>
#include "Ratio.h"
#include "StringLiteral.h"
#include "TypeUtils.h"
#include <string>

/**
 * UnitIdentifier - the unique representation of a particular unit. Comprised of atomic units and exponents
 * For instance: (meter, 1), (second, -2) -> acceleration (m/s^2)
 * These are constructed as a list of tuples of StringLiterals and ints
 */

template <StringLiteral Symbol, IsRatio Exponent>
struct UnitLeaf
{
    static constexpr StringLiteral symbol = Symbol;
    using exponent = Exponent;
};

/** Concept to match UnitLeaf */

// Define the primary template
template <typename T>
struct IsUnitLeafHelper : std::false_type
{
};

// Specialization for the `Unit` template
template <StringLiteral Symbol, IsRatio Exponent>
struct IsUnitLeafHelper<UnitLeaf<Symbol, Exponent>> : std::true_type
{
};

// Shorthand
template <typename T>
concept IsUnitLeaf = IsUnitLeafHelper<T>::value;

/** Basic definition of UnitLeafVector */

template <typename... Ts>
    requires(IsUnitLeaf<Ts> && ...)
struct UnitLeafVector
{
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
concept IsUnitLeafVector = IsUnitLeafVectorHelper<T>::value;

/** Concept to match UnitLeaves of same symbol */
template <typename A, typename B>
concept _IsSameSymbol = requires {
    A::symbol;
    B::symbol;
    requires StrEq<A::symbol, B::symbol>::value;
};

template <typename A, typename B>
concept ULIsSameSymbol = IsUnitLeaf<A> && IsUnitLeaf<B> && _IsSameSymbol<A, B>;

/** Concept to match unit leaf with exp 0 */

template <typename A>
concept ULIsZero_ = requires {
    typename A::exponent;
    requires RatioIsZero<typename A::exponent>;
};

template <typename A>
concept ULIsZero = IsUnitLeaf<A> && ULIsZero_<A>;

/** Comparison function */
template <typename A, typename B>
concept _CompareSymb = requires {
    A::symbol;
    B::symbol;
    requires CompareStrings<A::symbol, B::symbol>::value;
};

template <typename A, typename B>
concept ULCompare = IsUnitLeaf<A> && IsUnitLeaf<B> && _CompareSymb<A, B>;

// Comparison selector SmallerOf
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


/** Combine two unitleaves (and add their exponents) */
template <typename U, typename V>
    requires(IsUnitLeaf<U> && IsUnitLeaf<V>)
struct ULCombine_;

template <typename U, typename V>
    requires(IsUnitLeaf<U> && IsUnitLeaf<V> && ULIsSameSymbol<U, V>)
struct ULCombine_<U, V>
{
    using type = UnitLeaf<U::symbol, std::ratio_add<typename U::exponent, typename V::exponent>>;
};

template <typename U, typename V>
    requires(IsUnitLeaf<U> && IsUnitLeaf<V> && ULIsSameSymbol<U, V>)
using ULCombine = typename ULCombine_<U, V>::type;

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

/* popfront */
template <typename T>
    requires IsUnitLeafVector<T>
struct ULPopFront;

template <IsUnitLeaf H, IsUnitLeaf... T>
struct ULPopFront<UnitLeafVector<H, T...>>
{
    using type = UnitLeafVector<T...>;
};

/** concat two lists */

template <typename U, typename V>
    requires(IsUnitLeafVector<U> && IsUnitLeafVector<V>)
struct ULConcat_;

template <IsUnitLeaf... V1, IsUnitLeaf... V2>
struct ULConcat_<UnitLeafVector<V1...>, UnitLeafVector<V2...>>
{
    using type = UnitLeafVector<V1..., V2...>;
};

template <typename U, typename V>
    requires(IsUnitLeafVector<U> && IsUnitLeafVector<V>)
using ULConcat = typename ULConcat_<U, V>::type;

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

/** Invert a UnitLeaf */
template <IsUnitLeaf V>
using ULInvert = UnitLeaf<V::symbol, std::ratio_multiply<typename V::exponent, std::ratio<-1>>>;

// Invert a UnitLeafVector
template <typename V>
    requires(IsUnitLeafVector<V>)
struct InvertUnitLeafVector_
{
};

template <IsUnitLeaf H, IsUnitLeaf... T>
struct InvertUnitLeafVector_<UnitLeafVector<H, T...>>
{
    using type = ULPrepend<ULInvert<H>, typename InvertUnitLeafVector_<UnitLeafVector<T...>>::type>;
};

template <IsUnitLeaf... T>
    requires(sizeof...(T) == 0)
struct InvertUnitLeafVector_<UnitLeafVector<T...>>
{
    using type = UnitLeafVector<T...>;
};

template <typename V>
    requires(IsUnitLeafVector<V>)
using InvertUnitLeafVector = typename InvertUnitLeafVector_<V>::type;

/** Exponentiate: compute A^(some ratio) */

template<typename A, typename Exp> requires (IsUnitLeaf<A> && IsRatio<Exp>)
using ULExp = UnitLeaf<A::symbol, std::ratio_multiply<typename A::exponent, Exp>>;

// Exponentiate a UnitLeafVector
template <typename V, typename Exp>
    requires(IsUnitLeafVector<V> && IsRatio<Exp>)
struct ExpUnitLeafVector_
{
};

template <IsUnitLeaf H, IsUnitLeaf... T, IsRatio Exp>
struct ExpUnitLeafVector_<UnitLeafVector<H, T...>,  Exp>
{
    using type = ULPrepend<ULExp<H, Exp>, typename ExpUnitLeafVector_<UnitLeafVector<T...>, Exp>::type>;
};

template <IsUnitLeaf... T, IsRatio Exp>
    requires(sizeof...(T) == 0)
struct ExpUnitLeafVector_<UnitLeafVector<T...>, Exp>
{
    using type = UnitLeafVector<T...>;
};

template <typename V, typename Exp>
    requires(IsUnitLeafVector<V> && IsRatio<Exp>)
using ExpUnitLeafVector = typename ExpUnitLeafVector_<V, Exp>::type;

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

/** Define sort */

template <typename Prefix, typename Rest>
    requires(IsUnitLeafVector<Prefix> && IsUnitLeafVector<Rest>)
struct ULSortRecurse;

template <IsUnitLeaf... Prefix, IsUnitLeaf H, IsUnitLeaf... T>
struct ULSortRecurse<UnitLeafVector<Prefix...>, UnitLeafVector<H, T...>>
{
    using min = ULMin<UnitLeafVector<H, T...>>;
    using rest = ULRemoveFirst<min, UnitLeafVector<H, T...>>;
    using type = typename ULSortRecurse<ULAppend<min, UnitLeafVector<Prefix...>>, rest>::type;
};

template <IsUnitLeaf... Prefix, IsUnitLeaf... T>
    requires(sizeof...(T) == 0)
struct ULSortRecurse<UnitLeafVector<Prefix...>, UnitLeafVector<T...>>
{
    using type = UnitLeafVector<Prefix...>;
};

template <typename T>
struct ULSort_;

template <IsUnitLeaf... T>
struct ULSort_<UnitLeafVector<T...>>
{
    using type = typename ULSortRecurse<UnitLeafVector<>, UnitLeafVector<T...>>::type;
};

template <typename V>
    requires(IsUnitLeafVector<V>)
using ULSort = typename ULSort_<V>::type;

/** Merge duplicates in a list */

template <typename V>
    requires(IsUnitLeafVector<V>)
struct ULMerge_;

template <IsUnitLeaf H1, IsUnitLeaf H2, IsUnitLeaf... T>
    requires ULIsSameSymbol<H1, H2>
struct ULMerge_<UnitLeafVector<H1, H2, T...>> // if there are duplicates, merge
{
    using Combined = ULCombine<H1, H2>;
    using type = typename ULMerge_<UnitLeafVector<Combined, T...>>::type;
};

template <IsUnitLeaf H, IsUnitLeaf... T> // if there are no duplicates, go to next index
struct ULMerge_<UnitLeafVector<H, T...>>
{
    using type = ULPrepend<H, typename ULMerge_<UnitLeafVector<T...>>::type>;
};

template <IsUnitLeaf... T>
    requires(sizeof...(T) == 0) // if we've reached the end
struct ULMerge_<UnitLeafVector<T...>>
{
    using type = UnitLeafVector<T...>;
};

template <typename V>
    requires(IsUnitLeafVector<V>)
using ULMerge = typename ULMerge_<V>::type;

/** Remove exp 0 */
template <typename V>
    requires(IsUnitLeafVector<V>)
struct ULRemoveZero_;

template <IsUnitLeaf H, IsUnitLeaf... T>
    requires(ULIsZero<H>)
struct ULRemoveZero_<UnitLeafVector<H, T...>>
{
    using type = typename ULRemoveZero_<UnitLeafVector<T...>>::type;
};

template <IsUnitLeaf H, IsUnitLeaf... T>
    requires(!ULIsZero<H>)
struct ULRemoveZero_<UnitLeafVector<H, T...>>
{
    using type = ULPrepend<H, typename ULRemoveZero_<UnitLeafVector<T...>>::type>;
};

template <IsUnitLeaf... T>
    requires(sizeof...(T) == 0)
struct ULRemoveZero_<UnitLeafVector<T...>>
{
    using type = UnitLeafVector<T...>;
};

template <typename V>
    requires(IsUnitLeafVector<V>)
using ULRemoveZero = typename ULRemoveZero_<V>::type;

/** GetUnique - sort, combine exponents and remove zeros */
template <typename V>
    requires(IsUnitLeafVector<V>)
using ULGetUnique = ULRemoveZero<ULMerge<ULSort<V>>>;
