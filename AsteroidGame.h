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
    double rotSpeed = 0.010;

    using Coord = typename GameObject<Depth>::Coord;

    static_assert(Depth > 0);
    inline virtual void Update() override
    {
        this->SetPos(Get2DRotationMatrix(rotSpeed) * this->pos);

        // theta += 0.001;
        GameObject<Depth>::Update();
    };
    inline virtual void Draw() override {};

    Vector2<Worldspace> GetWorldpos()
    {
        return _GetWorldpos<Depth>(this, this->pos);
    }

private:
    // template <size_t PDepth>
    // struct WPosT_
    // {
    // };
    // template <size_t PDepth>
    //     requires(PDepth == 0)
    // struct WPosT_<PDepth>
    // {
    //     using type = Vector2<ObjCoord<0>>;
    // };
    // template <size_t PDepth>
    //     requires(PDepth >= 1)
    // struct WPosT_<PDepth>
    // {
    //     using type = Vector2<ObjCoord<PDepth - 1>>;
    // };
    // template <size_t PDepth>
    // using WPosT = typename WPosT_<PDepth>::type;

    template <size_t PDepth>
        requires(PDepth >= 0)
    inline auto _GetWorldpos(GameObject<PDepth> *go, Vector2<ObjCoord<PDepth>> &ppos)
    {
        if constexpr (PDepth == 0)
        {
            return ppos;
        }
        else
        {
            GameObject<PDepth - 1> *parent = go->GetParent();
            Vector2<ObjCoord<PDepth - 1>> res = parent->ApplyTransform(ppos);
            return _GetWorldpos<PDepth - 1>(parent, res);
        }
    }
};

class Asteroid : public AsciiWorldObject<kWrapBoth>
{
public:
    Asteroid(AsciiGraphics *asciiGraphics, double radius_)
        : AsciiWorldObject(asciiGraphics), radius(radius_) {};

    Child<Orbiter> *o1 = nullptr;
    Child<Orbiter>::Child<Orbiter> *o2 = nullptr;
    inline virtual void Initialize() override
    {
        x = 10_ws;
        y = 10_ws;

        o1 = AddChild<Orbiter>();
        o1->SetPos({0, 5});

        o2 = o1->AddChild<Orbiter>();
        o2->SetPos({0, 2});
        o2->rotSpeed = 0.03;
        // o1->SetPos(Get2DRotationMatrix(0.785398) * o1->GetPos());

        // vel = {0.2, 0.2};
    }

    inline virtual void Update() override
    {
        vel += acc * 1_frame;
        x += vel.x() * 1_frame;
        y += vel.y() * 1_frame;

        GameObject<>::Update();
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

        Vector2<Worldspace> wv2 = o2->GetWorldpos();
        chars[numChars++] = {
            .x = x + wv2.x(),
            .y = y + wv2.y(),
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
        CreateGameObject<Asteroid>(ascii, 2);
    }
};