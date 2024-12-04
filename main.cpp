#include "SnakeGame.h"
#include <iostream>

int main()
{

    std::vector<std::string> games = {"snake", "bird"};

    std::cout << "Choose a game:" << std::endl;
    for (uint i = 0; i < games.size(); i++)
    {
        std::cout << i << ": " << games[i] << std::endl;
    }
    uint selection = 0;
    std::cin >> selection;


    if (games[selection] == "snake")
    {
        return PlaySnakeGame();
    }

    // int keyBuf[10] = {0};
    // int ch;
    // usleep(1000 * 1000);
    // do
    // {
    //     showGames();
    //     // Display options
    //     // std::cout.flush();

    //     // Look for keypresses
    //     for (uint i = 9; i >= 1; i--)
    //     {
    //         keyBuf[i] = keyBuf[i - 1];
    //     }
    //     keyBuf[0] = ch;
    //     if (keyBuf[2] == 27 && keyBuf[1] == 91 && keyBuf[0] == 65)
    //     {
    //         std::cout << "Up pressed" << "\n\r";
    //     }
    //     else if (keyBuf[2] == 27 && keyBuf[1] == 91 && keyBuf[0] == 66)
    //     {
    //         std::cout << "Down pressed" << std::endl;
    //     }
    //     else if (keyBuf[2] == 27 && keyBuf[1] == 91 && keyBuf[0] == 67)
    //     {
    //         std::cout << "Right pressed" << std::endl;
    //     }
    //     else if (keyBuf[2] == 27 && keyBuf[1] == 91 && keyBuf[0] == 68)
    //     {
    //         std::cout << "Left pressed" << std::endl;
    //     }
    //     // std::cout << "Key pressed: " << ch << std::endl;
    //     // std::cout << "b" << keyBuf[0] << " " << keyBuf[1] << " " << keyBuf[2];
    // } while ((ch = getch()));

    // endwin(); // End ncurses mode
    return 0;
    // return SnakeGame();
}