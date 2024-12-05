#pragma once
#include "UnitLib/Unit.h"
#include "Game.h"
#include <iostream>
#include <cstdio>

//------------------------------------------------------------------------------
// Consts
//------------------------------------------------------------------------------

const uint BUF_SIZE = 2048;
const uint ESC_SIZE = 32;
const uint MAX_WIDTH = 1024;

enum TextAttribute
{
    kTextNormal = 0,
    kTextBold = 1,
    kTextUnderline = 4,
    kTextReverse = 7,
};

enum FGColor
{
    kFGBlack = 30,
    kFGRed = 31,
    kFGGreen = 32,
    kFGYellow = 33,
    kFGBlue = 34,
    kFGMagenta = 35,
    kFGCyan = 36,
    kFGWhite = 37
};

enum BGColor
{
    kBGNone = -1,
    kBGBlack = 40,
    kBGRed = 41,
    kBGGreen = 42,
    kBGYellow = 43,
    kBGBlue = 44,
    kBGMagenta = 45,
    kBGCyan = 46,
    kBGWhite = 47
};

//------------------------------------------------------------------------------
// CharPixel definition
//------------------------------------------------------------------------------

template <WrapType Wrap>
struct CharPixel
{
    WorldX<Wrap> x;
    WorldY<Wrap> y;
    char pix;
};
const size_t MAX_CHARS = 256;

template <typename T>
concept IsCharPixel = requires(T t) {
    { t.x } -> IsUnitWithSymbol<WORLDSPACE>;
    { t.y } -> IsUnitWithSymbol<WORLDSPACE>;
    { t.pix } -> std::convertible_to<char>;
};

//------------------------------------------------------------------------------
// AsciiGraphics definition
//------------------------------------------------------------------------------

class AsciiGraphics
{
public:
    /** @brief Constructor, takes and stores an ostream */
    AsciiGraphics(std::ostream &oss = std::cout) : os(&oss) {};

    /** @brief Clears the screen */
    void ClearScreen()
    {
        const char *CLEAR_SCREEN = "\033[2J";
        os->write(CLEAR_SCREEN, strlen(CLEAR_SCREEN));
    }

    /** @brief Moves the cursor to the position specified by x, y */
    void MoveCursor(const uint &x, const uint &y)
    {
        size_t n = snprintf(escBuf, BUF_SIZE, "\033[%d;%dH", y, x);
        os->write(escBuf, n);
    }

    /** @brief Writes text to the current cursor position */
    void Write(const std::string &text)
    {
        os->write(text.c_str(), text.size());
    }

    /** @brief Draws the character `char` to x, y */
    void DrawChar(const uint &x, const uint &y, const char &fill)
    {
        MoveCursor(x, y);
        os->put(fill);
    }

    /** @brief Draw a char pixel. Enforces Worldspace */
    template <IsCharPixel CP>
    void DrawCharPixel(CP &charPixel)
    {
        DrawChar(charPixel.x.GetValue(), charPixel.y.GetValue(), charPixel.pix);
    }

    /** @brief Draws the text `str` to x, y */
    void DrawText(const Worldspace x, const Worldspace y, const char *str)
    {
        MoveCursor(x.GetValue(), y.GetValue());
        os->write(str, strlen(str));
    }

    /** @brief Draws a rectangle (filled or not) using the given character */
    void DrawRect(const uint &x, const uint &y, const uint &width, const uint &height, const char &fill = '.', const bool filled = true)
    {
        assert(width < MAX_WIDTH);
        std::fill_n(rowBuf, width, fill);

        for (uint i = 0; i < height; i++)
        {
            MoveCursor(x, y + i);

            if (filled)
            {
                os->write(rowBuf, width);
            }
            else
            {
                if (i == 0 || i == height - 1)
                {
                    os->write(rowBuf, width);
                }
                else
                {
                    os->put(fill);
                    MoveCursor(x + width - 1, y + i);
                    os->put(fill);
                }
            }
        }
    }

    void SetTextColor(FGColor fg, BGColor bg = kBGNone, TextAttribute attribute = kTextNormal)
    {

        size_t n;
        if (bg != kBGNone)
        {
            n = snprintf(escBuf, BUF_SIZE, "\033[%d;%d;%dm", attribute, fg, bg);
        }
        else
        {
            n = snprintf(escBuf, BUF_SIZE, "\033[%d;%dm", attribute, fg);
        }
        os->write(escBuf, n);
    }

    void ResetTextColor()
    {
        const char *RESET_TEXT = "\033[0m";
        os->write(RESET_TEXT, strlen(RESET_TEXT));
    }

    /** @brief End a frame - flushes the output stream and resets cursor pos */
    void EndFrame()
    {
        MoveCursor(0, 0);
        os->flush();
    }

private:
    // Shared buffers useable by many functions
    // char buf[BUF_SIZE];
    char escBuf[ESC_SIZE];
    char rowBuf[MAX_WIDTH];

    // Underlying ostream of this AsciiGraphics
    std::ostream *os = nullptr;
};