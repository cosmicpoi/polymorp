#pragma once
#include <iostream>
#include <cstdio>

const uint BUF_SIZE = 2048;
const uint ESC_SIZE = 32;
const uint MAX_WIDTH = 1024;
const char *CLEAR_SCREEN = "\033[2J";
const size_t CLEAR_SCREEN_LEN = std::strlen(CLEAR_SCREEN);

class AsciiGraphics
{
public:
    // Constructor
    AsciiGraphics(std::ostream &oss = std::cout) : os(&oss) {};

    void ClearScreen()
    {
        os->write(CLEAR_SCREEN, CLEAR_SCREEN_LEN);
    }

    void MoveCursor(const uint &x, const uint &y)
    {
        // std::cout << "[start move cursor]";
        size_t n = snprintf(escBuf, BUF_SIZE, "\033[%d;%dH", y, x);
        // size_t n = snprintf(buf, BUF_SIZE, "[%d;%dH", y, x);
        os->write(escBuf, n);
        // std::cout << "[end move cursor]";
    }

    void Write(const std::string &text)
    {
        os->write(text.c_str(), text.size());
    }

    void DrawChar(const uint &x, const uint &y, const char &fill)
    {
        MoveCursor(x, y);
        os->put(fill);
    }

    void DrawText(const uint &x, const uint &y, const char *str)
    {
        MoveCursor(x, y);
        os->write(str, strlen(str));
    }

    void DrawRect(const uint &x, const uint &y, const uint &width, const uint &height, const char& fill = '.', const bool filled = true)
    {
        assert(width < MAX_WIDTH);;
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

    void EndFrame()
    {
        os->flush();
    }

private:
    // Shared buffers useable by many functions
    char buf[BUF_SIZE];
    char escBuf[ESC_SIZE];
    char rowBuf[MAX_WIDTH];

    // Underlying ostream of this AsciiGraphics
    std::ostream *os = nullptr;
};