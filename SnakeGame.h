#pragma once

#include "AsciiGame.h"
#include "unistd.h"

/**
 * Various game object implementatinos
 */
class Rect : public GameObject
{
public:
    Rect(uint width_, uint height_) : width(width_), height(height_) {};

    inline virtual void Update() override
    {
        x = x + 0.5;
        y = y + 0.2;
    }
    inline virtual void Draw(AsciiGraphics &ascii) override
    {
        // Set up chars
        numChars = 0;
        for (uint i = 0; i < height; i++)
        {
            for (uint j = 0; j < width; j++)
            {
                chars[numChars++] = {.x = x + j, .y = y + i, .pix = '.'};
            }
        }
        // Flush chars

        ascii.SetTextColor(kFGRed, kBGNone, kTextBold);
        DrawChars(ascii);
        ascii.ResetTextColor();
    }

private:
    uint width = 0;
    uint height = 0;
};

/**
 * Game code
 */
class SnakeGame : public AsciiGame
{
public:
    inline virtual void Initialize() override
    {
        CreateGameObject<Rect>(10, 10);
    }
};

/**
 * Game loop
 */
int PlaySnakeGame()
{
    return PlayGame<SnakeGame>();
}