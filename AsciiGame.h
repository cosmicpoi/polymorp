#pragma once

#include "AsciiGraphics.h"
#include "Game.h"

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr double DEFAULT_ASCII_WIDTH = 50;
constexpr double DEFAULT_ASCII_HEIGHT = 25;

//------------------------------------------------------------------------------
// GameObject definitions
//------------------------------------------------------------------------------

template <WrapType Wrap = kWrapNone>
class AsciiGameObject : public GameObject<Wrap>
{
public:
    AsciiGameObject(AsciiGraphics *asciiGraphics) : ascii{asciiGraphics} {};

    inline virtual void Draw() override
    {
        DrawChars();
    }

protected:
    inline void DrawChars()
    {
        for (uint i = 0; i < numChars; i++)
        {
            ascii->DrawCharPixel(chars[i]);
        }
    }
    // Member variables
    size_t numChars = 0;
    CharPixel<Wrap> chars[MAX_CHARS];

    AsciiGraphics *ascii = nullptr;
};

//------------------------------------------------------------------------------
// Game definition
//------------------------------------------------------------------------------

class AsciiGame : public Game
{
public:
    inline static double GET_DEFAULT_WIDTH() { return DEFAULT_ASCII_WIDTH; };
    inline static double GET_DEFAULT_HEIGHT() { return DEFAULT_ASCII_HEIGHT; };

    AsciiGame() : Game(), ascii{AsciiGraphics{}} {};

    inline virtual void Draw() override
    {
        ascii.ClearScreen();

        // Draw bounds
        ascii.DrawRect(XBounds::lowerBound, YBounds::lowerBound, XBounds::width(), YBounds::height(), '.', false);

        // Draw objects
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                gameObjects[i]->Draw();
            }
        }

        // End frame
        ascii.EndFrame();
    }

protected:
    AsciiGraphics ascii;
};

//------------------------------------------------------------------------------
// Game loop
//------------------------------------------------------------------------------

template <IsGame G>
    requires std::is_base_of_v<AsciiGame, G>
int PlayGame()
{
    XBounds::SetLowerBound(1);
    XBounds::SetUpperBound(1 + G::GET_DEFAULT_WIDTH());
    YBounds::SetLowerBound(1);
    YBounds::SetUpperBound(1 + G::GET_DEFAULT_HEIGHT());

    G *game = new G();

    game->Initialize();
    while (1)
    {
        game->Update();
        game->Draw();

        usleep(1000 * 16);
    }
    return 0;
}