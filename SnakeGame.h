#pragma once

#include "AsciiGraphics.h"
#include "GameTypes.h"
#include "unistd.h"

/**
 * Unit definitions
 */

struct XBounds
{
    static constexpr double lowerBound = 1;
    static constexpr double upperBound = 50;
    static constexpr double width() { return upperBound - lowerBound; };
};
using ClippedX = ClipDouble<XBounds>;

struct YBounds
{
    static constexpr double lowerBound = 1;
    static constexpr double upperBound = 25;
    static constexpr double height() { return upperBound - lowerBound; };
};
using ClippedY = ClipDouble<YBounds>;

constexpr char OBJECTSPACE[] = "worldspace";
constexpr char WORLDSPACE[] = "worldspace";

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
        DrawMake();
        DrawQueue(ascii);
    }

protected:
    inline virtual void DrawMake() {
    };

    size_t numChars = 0;
    CharPixel chars[MAX_CHARS];

private:
    inline virtual void DrawQueue(AsciiGraphics &ascii)
    {
        for (uint i = 0; i < numChars; i++)
        {
            const CharPixel &px = chars[i];
            ascii.DrawChar(px.x, px.y, px.pix);
        }
    }
};

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
    inline virtual void DrawMake() override
    {
        numChars = 0;
        for (uint i = 0; i < height; i++)
        {
            for (uint j = 0; j < width; j++)
            {
                chars[numChars++] = {.x = x + j, .y = y + i, .pix = '.'};
            }
        }
    }

private:
    uint width = 0;
    uint height = 0;
    ClippedX x = 3;
    ClippedY y = 3;
};

/**
 * Game code
 */
class SnakeGame
{
public:
    SnakeGame() : ascii{AsciiGraphics{}}
    {
        myRect = new Rect(10, 10);
    }
    ~SnakeGame()
    {
        delete myRect;
    }

    inline void Update()
    {
        myRect->Update();

        // double birdX = frameCount / 10;
        // double birdY = 10 + 5 * std::sin(frameCount / 30.0);

        // ascii.DrawText(std::round(birdX), std::round(birdY), "🐍");
        // ascii.EndFrame();

        frameCount++;
    }

    inline void Draw()
    {
        ascii.ClearScreen();

        // Draw bounds
        ascii.DrawRect(XBounds::lowerBound, YBounds::lowerBound, XBounds::width(), YBounds::height(), '.', false);

        // Draw objects

        ascii.SetTextColor(kFGRed, kBGNone, kTextBold);
        myRect->Draw(ascii);
        ascii.ResetTextColor();

        ascii.EndFrame();
    }

    void DrawGL()
    {
    }

    void DrawASCII()
    {
    }

private:
    Rect *myRect = nullptr;

    uint frameCount = 0;
    AsciiGraphics ascii;
};

/**
 * Game loop
 */
int PlaySnakeGame()
{
    SnakeGame game{};
    while (1)
    {
        game.Update();
        game.Draw();

        usleep(1000 * 16);
    }
}