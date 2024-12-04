#include "UnitLib/Unit.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Print.h"

#include "SnakeGame.h"
#include "GLTest.h"
#include <iostream>

#include "GameTypes.h"

using dUEmpty = EmptyUnit<double>;
using dUKilo = UnitMultRatio<dUEmpty, std::ratio<1000>>;

struct MyBounds
{
    static constexpr double lowerBound = 10;
    static constexpr double upperBound = 90;
};

using Clipped = ClipDouble<MyBounds>;

int main()
{

    // Vector2<double> v{100, 100};
    // Vector2<Clipped> cv = v;
    // std::cout << ConvertibleOrConstructible<Clipped, double> << std::endl;
    // std::cout << clip<MyBounds>(97) << std::endl;
    // ClipDouble<MyBounds> b{100};
    // b = 120;
    // std::cout << cv << std::endl;
    return PlaySnakeGame();
    // return RunGLTest();

    // std::vector<std::string> games = {"snake", "gltest"};

    // std::cout << "Choose a game:" << std::endl;
    // for (uint i = 0; i < games.size(); i++)
    // {
    //     std::cout << i << ": " << games[i] << std::endl;
    // }
    // uint selection = 0;
    // std::cin >> selection;

    // if (games[selection] == "snake")
    // {
    //     return PlaySnakeGame();
    // } else if (games[selection] == "gltest")
    // {
    //     return RunGLTest();
    // }

    // return 0;
}
