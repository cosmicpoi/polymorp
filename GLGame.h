#pragma once

#include "GLGraphics.h"
#include "Game.h"

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr double DEFAULT_GL_WIDTH = 500;
constexpr double DEFAULT_GL_HEIGHT = 400;

//------------------------------------------------------------------------------
// GameObject definition
//------------------------------------------------------------------------------

template <size_t Depth = 0>
class GLGameObject : public GameObject<Depth>
{
public:
    GLGameObject(GLGraphics *glGraphics) : gl{glGraphics} {};

    inline virtual void Draw() override
    {
    }

protected:
    GLGraphics *gl = nullptr;
};

//------------------------------------------------------------------------------
// Game definition
//------------------------------------------------------------------------------

class GLGame : public Game
{
public:
    inline static double GET_DEFAULT_WIDTH() { return DEFAULT_GL_WIDTH; };
    inline static double GET_DEFAULT_HEIGHT() { return DEFAULT_GL_HEIGHT; };

    GLGame(GLGraphics *glGraphics) : Game(), gl{glGraphics}
    {
        std::fill_n(gameObjects, MAX_GAME_OBJECTS, nullptr);

        if (!gl->Initialize(XBounds::width(), YBounds::height(), "Test Window"))
        {
            throw std::runtime_error("Could not create window");
            return;
        }
        if (!gl->BuildShaders())
        {
            throw std::runtime_error("Could not create shaders");
            return;
        }
    }

    inline virtual void Draw() override
    {
        gl->ClearScreen(1, 1, 1);

        // Draw objects
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                if (gameObjects[i]->IsEnabled())
                {
                    gameObjects[i]->Draw();
                }
            }
        }

        // std::cout << "gl end frame" << std::endl;
        // End frame
        gl->EndFrame();
    }

protected:
    GLGraphics *gl = nullptr;
};

//------------------------------------------------------------------------------
// Game loop
//------------------------------------------------------------------------------

template <IsGame G>
    requires std::is_base_of_v<GLGame, G>
int PlayGame()
{
    XBounds::SetLowerBound(0);
    XBounds::SetUpperBound(0 + G::GET_DEFAULT_WIDTH());
    YBounds::SetLowerBound(0);
    YBounds::SetUpperBound(0 + G::GET_DEFAULT_HEIGHT());

    GLGraphics gl{};
    G *game = new G(&gl);

    game->Initialize();
    while (!gl.ShouldClose())
    {
        gl.ProcessInput();

        game->Update();
        game->Draw();

        usleep(1000 * 16);
    }
    return 0;
}