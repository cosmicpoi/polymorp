#pragma once
#include <cassert>
#include "AsciiGraphics.h"
#include "unistd.h"

class SnakeGame
{
public:
    SnakeGame()
    {
    }

    inline void Initialize()
    {
        std::cout.flush();
        ascii = AsciiGraphics{};
    }
    inline void Update()
    {
        std::cout << "running" << std::endl;
        ascii.ClearScreen();

        double birdX = frameCount / 10;
        double birdY = 10 + 5 * std::sin(frameCount / 30.0);

        ascii.SetTextColor(kFGRed, kBGNone, kTextBold);
        ascii.DrawRect(1, 1, 40, 40, '.', false);
        ascii.ResetTextColor();

        ascii.DrawText(std::round(birdX), std::round(birdY), "🐍");

        ascii.EndFrame();

        ascii.EndFrame();

        frameCount++;
    }

    void DrawGL() 
    {

    }

    void DrawASCII()
    {

    }

private:
    uint frameCount = 0;
    AsciiGraphics ascii;
};

int PlaySnakeGame()
{
    SnakeGame game{};
    while (1)
    {
        game.Update();

        usleep(1000 * 16);
    }
}