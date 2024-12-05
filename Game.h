#pragma once

#include "GameTypes.h"
#include "UnitLib/Unit.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Matrix.h"
#include "Keypress.h"
#include <unistd.h>

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr size_t MAX_GAME_OBJECTS = 1024;
constexpr size_t MAX_CHILDREN = 16;
constexpr double DEFAULT_WIDTH = 320;
constexpr double DEFAULT_HEIGHT = 240;

constexpr char OBJECTSPACE[] = "objectspace";
constexpr char WORLDSPACE[] = "worldspace";
constexpr char FRAME[] = "frame";

//------------------------------------------------------------------------------
// Bounded type definnitions
//------------------------------------------------------------------------------

struct XBounds
{
    static void SetLowerBound(double lb) { lowerBound = lb; };
    static void SetUpperBound(double ub) { upperBound = ub; };
    inline static double upperBound;
    inline static double lowerBound;
    static double width() { return upperBound - lowerBound; };
};

struct YBounds
{
    static void SetLowerBound(double lb) { lowerBound = lb; };
    static void SetUpperBound(double ub) { upperBound = ub; };
    inline static double upperBound;
    inline static double lowerBound;
    static double height() { return upperBound - lowerBound; };
};
using ClippedX = ClipDouble<XBounds>;
using ClippedY = ClipDouble<YBounds>;

//------------------------------------------------------------------------------
// Unit definitions
//------------------------------------------------------------------------------

/**
 * Basic definitions
 */
using Objectspace = TypeAtomic<double, OBJECTSPACE>;
using Worldspace = TypeAtomic<double, WORLDSPACE>;
using Frame = TypeAtomic<double, FRAME>;
using World_per_Object = DivideType<Worldspace, Objectspace>;

/**
 * Vel/acc types
 */
template <typename Dist>
using VelType = DivideType<Dist, Frame>;

template <typename Dist>
using AccType = DivideType<Dist, ExpType<2, Frame>>;

using World_per_Frame = DivideType<Worldspace, Frame>;
using World_per_Frame_2 = DivideType<World_per_Frame, Frame>;

/**
 * Clip types
 */
using ClipWorldX = TypeAtomic<ClippedX, WORLDSPACE>;
using ClipWorldY = TypeAtomic<ClippedY, WORLDSPACE>;

/**
 * Literal operators
 */
Worldspace operator"" _ws(long double val)
{
    return Worldspace{val};
}

Worldspace operator"" _ws(unsigned long long val)
{
    return Worldspace{static_cast<long double>(val)};
}

Objectspace operator"" _os(long double val)
{
    return Objectspace{val};
}

Objectspace operator"" _os(unsigned long long val)
{
    return Objectspace{static_cast<long double>(val)};
}

Frame operator"" _frame(long double val)
{
    return Frame{val};
}

Frame operator"" _frame(unsigned long long val)
{
    return Frame{static_cast<long double>(val)};
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
    inline virtual void Initialize() {};
    inline virtual bool Collide(const Vector2<Worldspace> &) { return false; };
    inline virtual void Update() = 0;
    inline virtual void Draw() = 0;
    inline void Disable()
    {
        enabled = false;
    }
    inline bool IsEnabled()
    {
        return enabled;
    }

private:
    bool enabled = true;
};

template <size_t Depth>
    requires(Depth >= 0)
using ObjCoord = DivideType<Worldspace, ExpType<Depth, Objectspace>>;

template <size_t Depth = 0>
class GameObject : public Entity
{
public:
    static constexpr size_t GO_Depth = Depth;
    using Coord = ObjCoord<Depth>;

    template <template <size_t> class ChildObj>
    using Child = ChildObj<Depth + 1>;

    ~GameObject()
    {
        for (uint i = 0; i < MAX_CHILDREN; i++)
        {
            if (children[i] != nullptr)
            {
                delete children[i];
            }
        }
    };

    inline virtual void Update()
    {
        for (uint i = 0; i < MAX_CHILDREN; i++)
        {
            if (children[i] != nullptr)
            {
                children[i]->Update();
            }
        }
    }

    template <template <size_t> class ChildObj, typename... Args>
        requires std::is_base_of_v<GameObject<Depth + 1>, Child<ChildObj>> &&
                 std::is_constructible_v<Child<ChildObj>, Args...>
    Child<ChildObj> *AddChild(Args... argList)
    {
        for (uint i = 0; i < MAX_CHILDREN; i++)
        {
            if (children[i] == nullptr)
            {
                Child<ChildObj> *obj = new Child<ChildObj>(argList...);
                obj->parent = this;
                children[i] = obj;
                children[i]->Initialize();
                return obj;
            }
        }

        throw std::runtime_error("Too many children!");
    };

    inline Vector2<ObjCoord<Depth>> ApplyTransform(const Vector2<ObjCoord<Depth + 1>> &vec)
    {
        // static_assert((CanMultiply<T, Objectspace>));
        // static_assert((std::convertible_to<MultiplyType<T, Objectspace>, Coord>));
        Vector2<Coord> scaled = Get2DScaleMatrix<Objectspace>() * vec;
        scaled += this->GetPos();
        return scaled;
    };

    inline GameObject<Depth - 1> *GetParent()
    {
        return parent;
    }

    inline void SetPos(Vector2<Coord> pos_)
    {
        this->pos = pos_;
    }
    inline virtual Vector2<Coord> GetPos()
    {
        return pos;
    }

protected:
    Vector2<Coord> pos{};
    Vector2<VelType<Coord>> vel{};
    Vector2<AccType<Coord>> acc{};
    GameObject<Depth - 1> *parent = nullptr;
    Entity *children[MAX_GAME_OBJECTS] = {nullptr};

    friend class GameObject<Depth - 1>;
};

/** WrapGameObject definition */
// Top-level objects (depth 0) only
template <WrapType Wrap = kWrapNone>
struct WrapCoords
{
    using Xcoord = WorldX<Wrap>;
    using Ycoord = WorldY<Wrap>;

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
    };
    ~Game()
    {
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                delete gameObjects[i];
            }
        }
    };

    template <typename GameObj, typename... Args>
        requires IsEntity<GameObj> &&
                 std::is_constructible_v<GameObj, Args...>
    inline GameObj *CreateGameObject(Args... argList)
    {
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] == nullptr)
            {
                GameObj *obj = new GameObj(argList...);
                obj->Initialize();
                gameObjects[i] = obj;
                return obj;
            }
        }

        throw std::runtime_error("Too many game objects!");
        return nullptr;
    };

    inline virtual void Initialize() = 0;

    inline virtual void Update()
    {
        KeyEventManager::GetInstance().Update(frameCount);
        frameCount++;

        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                if (gameObjects[i]->IsEnabled())
                {
                    gameObjects[i]->Update();
                }
            }
        }

        UpdateEnd();
    };

    inline virtual void UpdateEnd() {};

    inline virtual void Draw() = 0;

protected:
    uint frameCount = 0;
    Entity *gameObjects[MAX_GAME_OBJECTS] = {nullptr};
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
    XBounds::SetLowerBound(0);
    XBounds::SetUpperBound(0 + G::GET_DEFAULT_WIDTH());
    YBounds::SetLowerBound(0);
    YBounds::SetUpperBound(0 + G::GET_DEFAULT_HEIGHT());

    game->Initialize();
    while (1)
    {
        game->Update();
        game->Draw();

        usleep(1000 * 16);
    }
    return 0;
}