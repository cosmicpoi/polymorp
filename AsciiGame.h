#pragma once

#include "AsciiGraphics.h"
#include "GameTypes.h"
#include "UnitLib/Unit.h"
#include "UnitLib/Vector.h"
#include <unistd.h>

/**
 * Consts
 */

constexpr size_t MAX_GAME_OBJECTS = 1024;

constexpr double DEFAULT_WIDTH = 50;
constexpr double DEFAULT_HEIGHT = 25;

constexpr char OBJECTSPACE[] = "worldspace";
constexpr char WORLDSPACE[] = "worldspace";

/**
 * Unit definitions
 */

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

using Objectspace = TypeAtomic<double, OBJECTSPACE>;
using Worldspace = TypeAtomic<double, WORLDSPACE>;
using World_per_Object = DivideType<Worldspace, Objectspace>;
using ClipWorldX = TypeAtomic<ClippedX, WORLDSPACE>;
using ClipWorldY = TypeAtomic<ClippedX, WORLDSPACE>;

/**
 * GameObject definition
 */

struct CharPixel
{
    ClippedX x;
    ClippedY y;
    char pix;
};
const size_t MAX_CHARS = 256;

class GameObject
{
public:
    GameObject() {};
    virtual ~GameObject() {};

    inline virtual void Update() {};

    inline virtual void Draw(AsciiGraphics &ascii)
    {
        DrawChars(ascii);
    }

protected:
    inline virtual void DrawChars(AsciiGraphics &ascii)
    {
        for (uint i = 0; i < numChars; i++)
        {
            const CharPixel &px = chars[i];
            ascii.DrawChar(px.x, px.y, px.pix);
        }
    }

    // Member variables
    size_t numChars = 0;
    CharPixel chars[MAX_CHARS];

    ClippedX x = 0;
    ClippedY y = 0;
};

template <typename T>
concept IsGameObject = std::is_base_of_v<GameObject, T>;
/**
 * Game definition
 */

class AsciiGame
{
public:
    AsciiGame() : ascii{AsciiGraphics{}}
    {
        std::fill_n(gameObjects, MAX_GAME_OBJECTS, nullptr);
    }
    ~AsciiGame()
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
        requires IsGameObject<GameObj> &&
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

    inline void Draw()
    {
        ascii.ClearScreen();

        // Draw bounds
        ascii.DrawRect(XBounds::lowerBound, YBounds::lowerBound, XBounds::width(), YBounds::height(), '.', false);

        // Draw objects
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                gameObjects[i]->Draw(ascii);
            }
        }

        // End frame
        ascii.EndFrame();
    }

private:
    GameObject *gameObjects[MAX_GAME_OBJECTS] = {nullptr};

    uint frameCount = 0;
    AsciiGraphics ascii;
};

template <typename T>
concept IsAsciiGame = std::is_base_of_v<AsciiGame, T>;

/**
 * Game loop
 */

template <typename G>
int PlayGame()
{
    G *game = new G();
    XBounds::SetLowerBound(1);
    XBounds::SetUpperBound(1 + DEFAULT_WIDTH);
    YBounds::SetLowerBound(1);
    YBounds::SetUpperBound(1 + DEFAULT_HEIGHT);

    game->Initialize();
    while (1)
    {
        game->Update();
        game->Draw();

        usleep(1000 * 16);
    }
    return 0;
}