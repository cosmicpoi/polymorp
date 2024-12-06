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
    Worldspace radius = 0.5;

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
        o1 = AddChild<Orbiter>();
        o1->SetPos({0, 5});
        o1->radius = fRand(1, 3);

        o2 = o1->AddChild<Orbiter>();
        o2->SetPos({0, 2});
        o2->rotSpeed = 0.03;
    }

    inline virtual void Update() override
    {
        vel += acc * 1_frame;
        x += vel.x() * 1_frame;
        y += vel.y() * 1_frame;

        radius *= 0.995;
        if (radius < 0.5_ws)
        {
            Disable();
        }

        GameObject<>::Update();
    }
    inline virtual void Draw() override
    {
        if (!IsEnabled())
        {
            numChars = 0;
            return;
        }

        numChars = 0;

        DrawCircle(x, y, radius);
        ascii->ResetTextColor();

        Vector2<Worldspace> wv = o1->GetWorldpos();
        DrawCircle(x + wv.x(), y + wv.y(), o1->radius);

        Vector2<Worldspace> wv2 = o2->GetWorldpos();
        DrawCircle(x + wv2.x(), y + wv2.y(), o2->radius);

        // Flush chars
        ascii->SetTextColor(kFGRed, kBGNone, kTextBold);
        DrawChars();
        ascii->ResetTextColor();
    }

    inline virtual bool Collide(const Vector2<Worldspace> &point) override
    {
        if (!IsEnabled())
        {
            return false;
        }

        bool c0 = (NormSquared(point - GetPos()) <= radius * radius);
        bool c1 = (NormSquared(point - o1->GetWorldpos()) <= o1->radius * o1->radius);
        bool c2 = (NormSquared(point - o2->GetWorldpos()) <= o2->radius * o2->radius);

        return c0 || c1 || c2;
    };

private:
    inline void DrawCircle(WorldX<kWrapBoth> cx, WorldY<kWrapBoth> cy, Worldspace rad)
    {
        for (Worldspace offX = -rad; offX <= rad; offX++)
        {
            for (Worldspace offY = -rad; offY <= rad; offY++)
            {
                if (NormSquared(Vector2<Worldspace>{offX, offY}) <= rad * rad)
                {
                    chars[numChars++] = {
                        .x = cx + offX,
                        .y = cy + offY,
                        .pix = '.'};
                }
            }
        }
    }

    Coord radius;
};

/**
 * Player code
 */

class Player : public AsciiWorldObject<kWrapBoth>
{
public:
    Player(AsciiGraphics *asciiGraphics)
        : AsciiWorldObject(asciiGraphics) {};

    static constexpr const char *PLAYER = "🐥";

    inline virtual void Initialize() override
    {
    }

    inline virtual void Update() override
    {
        double delX = 0;
        double delY = 0;

        if (KeyEventManager::GetInstance().Keypressed(kKeyCodeDown))
        {
            delY += 1;
        }
        if (KeyEventManager::GetInstance().Keypressed(kKeyCodeUp))
        {
            delY -= 1;
        }
        if (KeyEventManager::GetInstance().Keypressed(kKeyCodeRight))
        {
            delX += 1;
        }
        if (KeyEventManager::GetInstance().Keypressed(kKeyCodeLeft))
        {
            delX -= 1;
        }

        acc = {delX * 0.04, delY * 0.04};

        vel += acc * 1_frame;
        x += vel.x() * 1_frame;
        y += vel.y() * 1_frame;

        vel *= 0.95;

        // GameObject<>::Update();
    }
    inline virtual void Draw() override
    {
        // Set up chars
        ascii->DrawText({x}, {y}, PLAYER);
    }

private:
    Coord radius;
};

/**
 * Game code
 */
class AsteroidGame : public AsciiGame
{
public:
    AsteroidGame(AsciiGraphics *asciiGraphics) : AsciiGame(asciiGraphics) {};

    Player *player = nullptr;
    Asteroid *asteroid = nullptr;
    bool gameOver = false;

    double points = 0;

    inline virtual void Initialize() override
    {
        asteroid = CreateGameObject<Asteroid>(ascii, 2);
        asteroid->SetVel({fRand(-0.2, 0.2), fRand(-0.2, 0.2)});
        player = CreateGameObject<Player>(ascii);

        player->SetPos({GET_DEFAULT_WIDTH() / 2, GET_DEFAULT_HEIGHT() / 2});
    }

    inline virtual void UpdateEnd() override
    {
        if (!gameOver)
        {
            points += fRand(0, 0.01);
        }
        if (!asteroid->IsEnabled())
        {
            asteroid = CreateGameObject<Asteroid>(ascii, 2);
            asteroid->SetVel({fRand(-0.2, 0.2), fRand(-0.2, 0.2)});
        }

        if (asteroid->Collide(player->GetPos()))
        {
            player->Disable();
            // asteroid->Disable();
            gameOver = true;
        }
    }

    inline virtual void Draw() override
    {
        AsciiGame::Draw();


        if (gameOver)
        {
            ascii->SetTextColor(kFGRed, kBGYellow, kTextBold);
            ascii->DrawText({GET_DEFAULT_WIDTH() / 2 - 7}, {GET_DEFAULT_HEIGHT() / 2}, "   GAME OVER   ");
            ascii->ResetTextColor();
        }

        ascii->MoveCursor(0, 0);
        std::cout << "Points: " << (int) points;

        ascii->EndFrame();
    }
};