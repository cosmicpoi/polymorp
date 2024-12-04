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

template <WrapType Wrap = kWrapNone>
class AsciiGameObject : public GameObject<Wrap>
{
public:
    AsciiGameObject(AsciiGraphics* asciiGraphics) : ascii{asciiGraphics} {};

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

/**
 * Game definition
 */

class AsciiGame : public Game
{
public:
    inline static double GET_DEFAULT_WIDTH() { return DEFAULT_ASCII_WIDTH; };
    inline static double GET_DEFAULT_HEIGHT() { return DEFAULT_ASCII_HEIGHT; };

    AsciiGame() : Game(), ascii{AsciiGraphics{}}
    {
        std::fill_n(gameObjects, MAX_GAME_OBJECTS, nullptr);
    }

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