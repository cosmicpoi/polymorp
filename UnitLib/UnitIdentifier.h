#pragma once

#include "UnitLeaf.h"

/** Define UnitIdentifier, a concept that specifies whether a UnitLeafVector mathces ULGetUnique */
template <typename V>
concept UnitIdentifier = IsUnitLeafVector<V> && std::is_same_v<V, ULGetUnique<V>>;

/* Also, alias UnitLeaf for a more intuitive name */
template <StringLiteral Symbol, IsRatio Exponent>
using UnitBase = UnitLeaf<Symbol, Exponent>;

/** Empty UID (unitless scalars) */
using EmptyUid = UnitLeafVector<>;

template <typename T>
concept IsEmptyUid = std::is_same_v<T, EmptyUid>;

/** Shorthand to make a UnitIdentifier */
template <typename... T>
using MakeUnitIdentifier = ULGetUnique<UnitLeafVector<T...>>;

/** Shorthand to make a UnitBase with exp 1 */
template <StringLiteral Symbol>
using UnitAtomic = MakeUnitIdentifier<UnitBase<Symbol, std::ratio<1>>>;

/**
 * Type transformers
 */

/** @brief Get reciprocal of a UnitIdentifier */
template <UnitIdentifier V>
using UIInvert = InvertUnitLeafVector<V>;

/** @brief Multiply two UnitIdentifiers */
template <UnitIdentifier A, UnitIdentifier B>
using UIMult = ULGetUnique<ULConcat<A, B>>;

/** @brief Divide two UnitIdentifiers */
template <UnitIdentifier A, UnitIdentifier B>
using UIDivide = ULGetUnique<ULConcat<A, UIInvert<B>>>;

/** @brief Exponentiate a UnitIdentifier, i.e. compute UID^(some ratio) */
template <UnitIdentifier A, IsRatio Exp>
using UIExp = ExpUnitLeafVector<A, Exp>;
