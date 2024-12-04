#include "UnitLib/Unit.h"
#include "UnitLib/Vector.h"
#include "UnitLib/Print.h"

#include "SnakeGame.h"
#include "GLGraphics.h"
#include "Triangle.h"
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
    std::vector<std::string> games = {"snake", "gl"};

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
    } else if (games[selection] == "gl")
    {
        GLGraphics gl;
        const unsigned int SCR_WIDTH = 800;
        const unsigned int SCR_HEIGHT = 600;
        Vector3<float> backGroundColor = {0.3f, 0.3f, 0.4f};
    
        if (!gl.Initialize(SCR_WIDTH, SCR_HEIGHT, "Test Window")) {
            return -1;
        }
        if(!gl.BuildShaders()){
            return -1;
        }
        float i = 0.001;
        while (!gl.ShouldClose()) {
            
            gl.ProcessInput();

            // give background screen a color
            gl.ClearScreen(backGroundColor[0], backGroundColor[1], backGroundColor[2]);
            
            //Draw a triangle
            /*
            float vertices[] = {
            -0.5f, -0.5f, 0.0f, // left  
            0.5f, -0.5f, 0.0f, // right 
            0.0f,  0.5f, 0.0f  // top   
            }; */

            Vector3<float> p1{-0.5f + i, -0.5f + i, 0.0f};
            Vector3<float> p2{ 0.5f + + i, -0.5f + i, 0.0f};
            Vector3<float> p3{0.0f,  0.5f + i, 0.0f};
            Triangle triangle(p1, p2, p3);

            gl.DrawTriangle(triangle);

            gl.EndFrame();
            //i = i+0.001;
        }
    }

    return 0;
}
