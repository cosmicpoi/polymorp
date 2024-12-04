#pragma once

#include "GLGame.h"

//------------------------------------------------------------------------------
// GameObjects
//------------------------------------------------------------------------------

class Tri : public GLGameObject<>
{
public:
    Tri(GLGraphics *glGraphics) : GLGameObject(glGraphics) {};

    inline virtual void Update() override
    {
        frameCount++;
        float i = (float)frameCount / 100.0f;
        p1 = Vector3<float>{-0.2f + i, -0.2f + i, 0.0f};
        p2 = Vector3<float>{0.2f + i, -0.2f + i, 0.0f};
        p3 = Vector3<float>{0.0f, 0.7f + i, 0.0f};
    };
    inline virtual void Draw() override
    {
        Triangle triangle(p1, p2, p3);
        // x, y, width, height
        Vector4<float> rect{-0.2f, -0.2f, 0.3f, 0.3f};

        gl->DrawTriangle(triangle);

        gl->DrawRectangle(rect);
    };

private:
    uint frameCount = 0;
    Vector3<float> p1;
    Vector3<float> p2;
    Vector3<float> p3;
};

//------------------------------------------------------------------------------
// Game code
//------------------------------------------------------------------------------

class JumpGame : public GLGame
{
public:
    JumpGame(GLGraphics *glGraphics) : GLGame(glGraphics) {};

    inline virtual void Initialize() override
    {
        CreateGameObject<Tri>(gl);
    };
};