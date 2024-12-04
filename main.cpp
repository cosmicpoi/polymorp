#include "SnakeGame.h"
#include "GLGraphics.h"
#include "Triangle.h"
#include "UnitLib/Vector.h"
#include <iostream>

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
            
            Vector3<float> p1{-0.2f + i, -0.2f + i, 0.0f};
            Vector3<float> p2{ 0.2f + i, -0.2f + i, 0.0f};
            Vector3<float> p3{0.0f,  0.7f + i, 0.0f};
            i=i+0.001;

            Triangle triangle(p1, p2, p3);
            
            // x, y, width, height
            Vector4<float> rect{-0.2f, -0.2f, 0.3f, 0.3f};

            gl.DrawTriangle(triangle);

            gl.DrawRectangle(rect);

            gl.EndFrame();
        }
    }

    return 0;
}
