#pragma once

#include "GameTypes.h"
#include "UnitLib/Unit.h"
#include "UnitLib/Vector.h"

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr size_t MAX_GAME_OBJECTS = 1024;



constexpr char OBJECTSPACE[] = "worldspace";
constexpr char WORLDSPACE[] = "worldspace";

//------------------------------------------------------------------------------
// Bounded type definnitions
//------------------------------------------------------------------------------

struct Bounds
{
    static void SetLowerBound(double lb) { lowerBound = lb; };
    static void SetUpperBound(double ub) { upperBound = ub; };
    inline static double upperBound;
    inline static double lowerBound;
};

struct XBounds : public Bounds
{
    static double width() { return upperBound - lowerBound; };
};
struct YBounds : public Bounds
{
    static double height() { return upperBound - lowerBound; };
};
using ClippedX = ClipDouble<XBounds>;
using ClippedY = ClipDouble<YBounds>;

//------------------------------------------------------------------------------
// Unit definitions
//------------------------------------------------------------------------------

using Objectspace = TypeAtomic<double, OBJECTSPACE>;
using Worldspace = TypeAtomic<double, WORLDSPACE>;
using World_per_Object = DivideType<Worldspace, Objectspace>;
using ClipWorldX = TypeAtomic<ClippedX, WORLDSPACE>;
using ClipWorldY = TypeAtomic<ClippedX, WORLDSPACE>;

Worldspace operator"" _ws(long double val)
{
    return Worldspace{val};
}

Objectspace operator"" _os(long double val)
{
    return Objectspace{val};
}

//------------------------------------------------------------------------------
// WorldType definitions
//------------------------------------------------------------------------------

enum WrapType
{
    kWrapNone,
    kWrapX,
    kWrapY,
    kWrapBoth,
};

template <WrapType Wrap>
struct WorldTypes
{
};

template <WrapType Wrap>
    requires(Wrap == kWrapNone)
struct WorldTypes<Wrap>
{
    using worldX = Worldspace;
    using worldY = Worldspace;
};

template <WrapType Wrap>
    requires(Wrap == kWrapX)
struct WorldTypes<Wrap>
{
    using worldX = ClipWorldX;
    using worldY = Worldspace;
};

template <WrapType Wrap>
    requires(Wrap == kWrapY)
struct WorldTypes<Wrap>
{
    using worldX = Worldspace;
    using worldY = ClipWorldY;
};

template <WrapType Wrap>
    requires(Wrap == kWrapBoth)
struct WorldTypes<Wrap>
{
    using worldX = ClipWorldX;
    using worldY = ClipWorldY;
};
template <WrapType Wrap>
using WorldX = typename WorldTypes<Wrap>::worldX;
template <WrapType Wrap>
using WorldY = typename WorldTypes<Wrap>::worldY;