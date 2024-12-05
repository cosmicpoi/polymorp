#include "AsteroidGame.h"
#include "JumpGame.h"
#include <iostream>

int main()
{
    std::vector<std::string> games = {"asteroid", "jump"};

    std::cout << "Choose a game:" << std::endl;
    for (uint i = 0; i < games.size(); i++)
    {
        std::cout << i << ": " << games[i] << std::endl;
    }
    uint selection = 0;
    std::cin >> selection;

    if (games[selection] == "asteroid")
    {
        return PlayGame<AsteroidGame>();
    }
    else if (games[selection] == "jump")
    {
        return PlayGame<JumpGame>();
    }

    return 0;
}
