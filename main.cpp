#include "SnakeGame.h"
#include "GLGraphics.h"
#include "Triangle.h"
#include "UnitLib/Vector.h"
#include <iostream>

int main()
{
    GLGraphics gl;
    const unsigned int SCR_WIDTH = 800;
    const unsigned int SCR_HEIGHT = 600;
    Vector3<float> backGroundColor = {0.3f, 0.3f, 0.4f};
   
    if (!gl.Initialize(SCR_WIDTH, SCR_HEIGHT, "Test Windwo")) {
        return -1;
    }
    if(!gl.BuildShaders()){
        return -1;
    }
    while (!gl.ShouldClose()) {

        gl.ProcessScreen(backGroundColor[0], backGroundColor[1], backGroundColor[2]);
        
        //Draw a triangle
        /*
        float vertices[] = {
        -0.5f, -0.5f, 0.0f, // left  
         0.5f, -0.5f, 0.0f, // right 
         0.0f,  0.5f, 0.0f  // top   
        }; */
        
        Vector3<float> p1{-0.5f, -0.5f, 0.0f};
        Vector3<float> p2{ 0.5f, -0.5f, 0.0f};
        Vector3<float> p3{0.0f,  0.5f, 0.0f};

        Triangle triangle(p1, p2, p3);

        gl.DrawTriangle(triangle);

        gl.EndFrame();
    }

    return 0;

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
