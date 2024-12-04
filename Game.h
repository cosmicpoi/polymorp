#pragma once

#include "GameTypes.h"
#include "UnitLib/Unit.h"
#include "UnitLib/Vector.h"
#include <unistd.h>

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr size_t MAX_GAME_OBJECTS = 1024;
constexpr double DEFAULT_WIDTH = 320;
constexpr double DEFAULT_HEIGHT = 240;

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

//------------------------------------------------------------------------------
// GameObject class
//------------------------------------------------------------------------------

class Entity
{
public:
    Entity() {};
    virtual ~Entity() {};
    inline virtual void Update() = 0;
    inline virtual void Draw() = 0;
};

template <WrapType Wrap = kWrapNone>
class GameObject : public Entity
{
protected:
    WorldX<Wrap> x{0};
    WorldY<Wrap> y{0};
};

template <typename T>
concept IsEntity = std::is_base_of_v<Entity, T>;

//------------------------------------------------------------------------------
// Game class
//------------------------------------------------------------------------------

class Game
{
public:
    inline static double GET_DEFAULT_WIDTH() { return DEFAULT_WIDTH; };
    inline static double GET_DEFAULT_HEIGHT() { return DEFAULT_HEIGHT; };

    Game()
    {
        std::fill_n(gameObjects, MAX_GAME_OBJECTS, nullptr);
    }
    ~Game()
    {
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                delete gameObjects[i];
            }
        }
    }

    template <typename GameObj, typename... Args>
        requires IsEntity<GameObj> &&
                 std::is_constructible_v<GameObj, Args...>
    inline void CreateGameObject(Args... argList)
    {
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] == nullptr)
            {
                gameObjects[i] = new GameObj(argList...);
                return;
            }
        }

        throw std::runtime_error("Too many game objects!");
    }

    inline virtual void Initialize() = 0;

    inline void Update()
    {
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                gameObjects[i]->Update();
            }
        }

        frameCount++;
    }

    inline virtual void Draw() = 0;

protected:
    Entity *gameObjects[MAX_GAME_OBJECTS] = {nullptr};
    uint frameCount = 0;
};

template <typename T>
concept IsGame = std::is_base_of_v<Game, T>;

/**
 * Game loop
 */

template <IsGame G>
int PlayGame()
{
    G *game = new G();
    XBounds::SetLowerBound(1);
    XBounds::SetUpperBound(1 + G::GET_DEFAULT_WIDTH());
    YBounds::SetLowerBound(1);
    YBounds::SetUpperBound(1 + G::GET_DEFAULT_HEIGHT());

    game->Initialize();
    while (1)
    {
        game->Update();
        game->Draw();

        usleep(1000 * 16);
    }
    return 0;
}