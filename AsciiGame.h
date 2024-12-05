#pragma once

#include "AsciiGraphics.h"
#include "GLGraphics.h"
#include "Game.h"

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

constexpr double DEFAULT_ASCII_WIDTH = 50;
constexpr double DEFAULT_ASCII_HEIGHT = 25;

//------------------------------------------------------------------------------
// GameObject definitions
//------------------------------------------------------------------------------

template <WrapType Wrap = kWrapBoth>
class AsciiWorldObject : public GameObject<0>
{
public:
    AsciiWorldObject(AsciiGraphics *asciiGraphics) : ascii{asciiGraphics} {};

    inline virtual void Draw() override
    {
        DrawChars();
    }

protected:
    inline void DrawChars()
    {
        for (uint i = 0; i < numChars; i++)
        {
            ascii->DrawCharPixel(chars[i]);
        }
    }
    // Member variables
    size_t numChars = 0;
    CharPixel<Wrap> chars[MAX_CHARS];

    AsciiGraphics *ascii = nullptr;
};

//------------------------------------------------------------------------------
// Game definition
//------------------------------------------------------------------------------

class AsciiGame : public Game
{
public:
    inline static double GET_DEFAULT_WIDTH() { return DEFAULT_ASCII_WIDTH; };
    inline static double GET_DEFAULT_HEIGHT() { return DEFAULT_ASCII_HEIGHT; };

    AsciiGame(AsciiGraphics *asciiGraphics) : Game(), ascii{asciiGraphics} {};

    inline virtual void Draw() override
    {
        ascii->ClearScreen();

        ascii->MoveCursor(60, 2);
        std::cout << KeyEventManager::GetInstance().Keydown(kKeyCodeDown);
        ascii->MoveCursor(60, 3);
        std::cout << KeyEventManager::GetInstance().Keyup(kKeyCodeDown);

        // Draw bounds
        ascii->DrawRect(XBounds::lowerBound, YBounds::lowerBound, XBounds::width(), YBounds::height(), '.', false);

        // Draw objects
        for (uint i = 0; i < MAX_GAME_OBJECTS; i++)
        {
            if (gameObjects[i] != nullptr)
            {
                gameObjects[i]->Draw();
            }
        }

        // End frame
        ascii->EndFrame();
    }

protected:
    AsciiGraphics *ascii;
};

//------------------------------------------------------------------------------
// Game loop
//------------------------------------------------------------------------------

template <IsGame G>
    requires std::is_base_of_v<AsciiGame, G>
int PlayGame()
{
    XBounds::SetLowerBound(1);
    XBounds::SetUpperBound(1 + G::GET_DEFAULT_WIDTH());
    YBounds::SetLowerBound(1);
    YBounds::SetUpperBound(1 + G::GET_DEFAULT_HEIGHT());

    AsciiGraphics ascii{};
    GLGraphics glHeadless{};
    G *game = new G(&ascii);

    glHeadless.InitializeHeadless();

    game->Initialize();
    while (1)
    {
        glHeadless.UpdateHeadless();
        game->Update();
        game->Draw();
        if ( KeyEventManager::GetInstance().Keydown(kKeyCodeDown))
        {
            std::cout << "key down" << std::endl;
        }

        usleep(1000 * 16);
    }
    return 0;
}