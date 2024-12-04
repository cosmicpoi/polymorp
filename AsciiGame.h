#pragma once

#include "AsciiGraphics.h"
#include "Game.h"

#include <unistd.h>

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr double DEFAULT_ASCII_WIDTH = 50;
constexpr double DEFAULT_ASCII_HEIGHT = 25;

//------------------------------------------------------------------------------
// GameObject definitions
//------------------------------------------------------------------------------

// class
class Entity
{
public:
    Entity() {};
    virtual ~Entity() {};
    inline virtual void Update() = 0;
    inline virtual void Draw(AsciiGraphics &ascii) = 0;
};

template <WrapType Wrap = kWrapNone>
class GameObject : public Entity
{
public:
    GameObject() {};
    virtual ~GameObject() override {};

    inline virtual void Update() override {};

    inline virtual void Draw(AsciiGraphics &ascii) override
    {
        DrawChars(ascii);
    }

protected:
    inline virtual void DrawChars(AsciiGraphics &ascii)
    {
        for (uint i = 0; i < numChars; i++)
        {
            ascii.DrawCharPixel(chars[i]);
        }
    }

    // Member variables
    size_t numChars = 0;
    CharPixel<Wrap> chars[MAX_CHARS];

    WorldX<Wrap> x{0};
    WorldY<Wrap> y{0};
};

template <typename T>
concept IsEntity = std::is_base_of_v<Entity, T>;
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
    Entity *gameObjects[MAX_GAME_OBJECTS] = {nullptr};

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
    XBounds::SetUpperBound(1 + DEFAULT_ASCII_WIDTH);
    YBounds::SetLowerBound(1);
    YBounds::SetUpperBound(1 + DEFAULT_ASCII_HEIGHT);

    game->Initialize();
    while (1)
    {
        game->Update();
        game->Draw();

        usleep(1000 * 16);
    }
    return 0;
}