#pragma once

#include "AsciiGame.h"

/**
 * Various game object implementatinos
 */
class Rect : public AsciiGameObject<kWrapBoth>
{
public:
    Rect(AsciiGraphics *asciiGraphics, uint width_, uint height_)
        : AsciiGameObject(asciiGraphics), width(width_), height(height_) {};

    inline virtual void Update() override
    {
        x = x + 0.5_ws;
        y = y + 0.2_ws;
    }
    inline virtual void Draw() override
    {
        // Set up chars
        numChars = 0;
        for (uint i = 0; i < height; i++)
        {
            for (uint j = 0; j < width; j++)
            {
                chars[numChars++] = {
                    .x = x + Worldspace{j},
                    .y = y + Worldspace{i},
                    .pix = '.'};
            }
        }

        // Flush chars
        ascii->SetTextColor(kFGRed, kBGNone, kTextBold);
        DrawChars();
        ascii->ResetTextColor();
    }

private:
    uint width = 0;
    uint height = 0;
};

/**
 * Game code
 */
class AsteroidGame : public AsciiGame
{
public:
    AsteroidGame(AsciiGraphics *asciiGraphics) : AsciiGame(asciiGraphics) {};

    inline virtual void Initialize() override
    {
        CreateGameObject<Rect>(ascii, 10, 10);
    }
};