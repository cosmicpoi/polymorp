#pragma once

#include "AsciiGame.h"
#include "UnitLib/Print.h"

/**
 * Various game object implementatinos
 */
template <size_t Depth>
class Orbiter : public GameObject<Depth>
{
public:
    using Coord = typename GameObject<Depth>::Coord;

    static_assert(Depth > 0);
    inline virtual void Update() override {};
    inline virtual void Draw() override {};

    Vector2<Worldspace> GetWorldpos()
    {
        return _GetWorldpos<Depth>(this->pos);
    }

private:
    template <size_t PDepth>
    Vector2<ObjCoord<PDepth - 1>> _GetWorldpos(Vector2<ObjCoord<PDepth>> &ppos)
    {
        if constexpr (PDepth == 1)
        {
            static_assert(std::is_same_v<ObjCoord<PDepth - 1>, Worldspace>);
            return this->GetParent()->ApplyTransform(ppos);
        }
        else
        {
            return _GetWorldpos<PDepth - 1>(this->GetParent()->ApplyTransform(ppos));
        }
    }
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
        o1->SetPos({0, -5});

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

        Vector2<Worldspace> wv = o1->GetWorldpos();
        chars[numChars++] = {
            .x = x + wv.x(),
            .y = y + wv.y(),
            .pix = '.'};

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
        CreateGameObject<Asteroid>(ascii, 3);
    }
};