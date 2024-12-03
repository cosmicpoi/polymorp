#pragma once

#include "AsciiGraphics.h"
#include "unistd.h"

// class SnakeGame
// {
//     void Update()
//     {

//     }

//     void Draw()
//     {

//     }
// }

int SnakeGame()
{
    AsciiGraphics ascii{};
    ascii.ClearScreen();
    ascii.MoveCursor(5, 5);
    // ascii.DrawRect(1, 1, 5, 5, "✊", false);

    // ascii.DrawChar(10, 10, 'h');
    // ascii.EndFrame();

    uint frameCount = 0;
    while (1)
    {
        ascii.ClearScreen();

        double birdX = frameCount / 10;
        double birdY = 10 + 5 * std::sin(frameCount / 30.0);

        ascii.SetTextColor(kFGRed, kBGNone, kTextBold);
        ascii.DrawRect(1, 1, 40, 40, '.', false);
        ascii.ResetTextColor();

        ascii.DrawText(std::round(birdX), std::round(birdY), "🐍");

        frameCount++;
        ascii.EndFrame();
        usleep(1000 * 16);
    }
    ascii.EndFrame();

    return 0;
}