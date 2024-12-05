#pragma once

#include "AsciiGame.h"
#include "UnitLib/Print.h"

/**
 * Various game object implementatinos
 */
template <size_t Depth>
class Orbiter : public GameObject<Depth>
{
    inline virtual void Update() override {};
    inline virtual void Draw() override {};
};

class Asteroid : public AsciiWorldObject<kWrapBoth>
{
public:
    Asteroid(AsciiGraphics *asciiGraphics, double radius_)
        : AsciiWorldObject(asciiGraphics), radius(radius_) {};

    Child<Orbiter> *o1 = nullptr;
    inline virtual void Initialize() override
    {
        o1 = AddChild<Orbiter>();

        vel = {0.2, 0.2};
    }

    inline virtual void Update() override
    {
        vel += acc * 1_frame;
        x += vel.x() * 1_frame;
        y += vel.y() * 1_frame;
    }
    inline virtual void Draw() override
    {
        // Set up chars

        numChars = 0;
        for (Worldspace offX = -radius; offX <= radius; offX++)
        {
            for (Worldspace offY = -radius; offY <= radius; offY++)
            {
                if (NormSquared(Vector2<Worldspace>{offX, offY}) <= radius * radius)
                {
                    chars[numChars++] = {
                        .x = x + offX,
                        .y = y + offY,
                        .pix = '.'};
                }
            }
        }

        // Flush chars
        ascii->SetTextColor(kFGRed, kBGNone, kTextBold);
        DrawChars();
        ascii->ResetTextColor();
    }

private:
    Coord radius;
    WorldY<kWrapBoth> y{0};
    WorldX<kWrapBoth> x{0};
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
        CreateGameObject<Asteroid>(ascii, 5);
    }
};