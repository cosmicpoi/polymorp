#pragma once

#include "UnitLeaf.h"

/** Define UnitIdentifier, a concept that specifies whether a UnitLeafVector mathces ULGetUnique */
template <typename V>
concept UnitIdentifier = IsUnitLeafVector<V> && std::is_same_v<V, ULGetUnique<V>>;

/* Also, alias UnitLeaf for a more intuitive name */
template <StringLiteral Symbol, int Exponent>
using UnitBase = UnitLeaf<Symbol, Exponent>;

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