#pragma once

#include "UnitLeaf.h"

/** Define UnitIdentifier, a concept that specifies whether a UnitLeafVector mathces ULGetUnique */
template <typename V>
concept UnitIdentifier = IsUnitLeafVector<V> && std::is_same_v<V, ULGetUnique<V>>;

/* Also, alias UnitLeaf for a more intuitive name */
template <StringLiteral Symbol, IsRatio Exponent>
using UnitBase = UnitLeaf<Symbol, Exponent>;

/** Shorthand to make a UnitBase with exp 1 */
template <StringLiteral Symbol>
using UnitAtomic = UnitBase<Symbol, std::ratio<1>>;


/** Other aliases */
template <UnitIdentifier V>
using UIInvert = InvertUnitLeafVector<V>;

/** Shorthand to make a UnitIdentifier */
template<typename... T> 
using MakeUnitIdentifier = ULGetUnique<UnitLeafVector<T...>>;

/** Multiply two UnitIdentifiers */
template <UnitIdentifier A, UnitIdentifier B>
using UIMult = ULGetUnique<ULConcat<A, B>>;

/** Divide = compute A / B */
template <UnitIdentifier A, UnitIdentifier B>
using UIDivide = ULGetUnique<ULConcat<A, UIInvert<B>>>;

/** Exponentiate: compute UID^(some ratio) */
template <UnitIdentifier A, IsRatio Exp>
using UIExp = ExpUnitLeafVector<A, Exp>;
