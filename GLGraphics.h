#pragma once

#define GL_SILENCE_DEPRECATION
#define MAX_TRIANGLES 200

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "Keypress.h"
#include "Triangle.h"
#include <string>
#include <iostream>

class GLGraphics {
public:
    GLGraphics() : window(nullptr), width(800), height(600), title("GLGraphics") {}
    ~GLGraphics() { Terminate(); }

    static void framebuffer_size_callback(GLFWwindow*, int width, int height){ glViewport(0, 0, width, height); }
    
    // Initialize headless window (for tracking keypresses)
    bool InitializeHeadless()
    {
        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW" << std::endl;
            return false;
        }
        GLFWwindow* window = glfwCreateWindow(1, 1, "Invisible Window", nullptr, nullptr);
        glfwSetWindowPos(window, 0, 0);
        if (!window) {
            glfwTerminate();
            std::cerr << "Failed to create GLFW window" << std::endl;
            return false;
        }
        glfwSetKeyCallback(window, key_callback);



        return true;
    }

    void UpdateHeadless()
    {
        glfwPollEvents();
    }

    // Initialize GLFW and OpenGL context
    bool Initialize(int w, int h, const std::string &windowTitle) {
        width = w;
        height = h;
        title = windowTitle;

        if (!glfwInit())
        {
            std::cout << "Failed to initialize GLFW" << std::endl;
            return false;
        }

        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        #ifdef __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        #endif

        window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            return false;
        }
        

        glfwMakeContextCurrent(window);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        glfwSetKeyCallback(window, key_callback);


        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cout << "Failed to initialize GLAD" << std::endl;
            return false;
        }

        glViewport(0, 0, width, height);
        return true;
    }

    bool BuildShaders(){
        unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
        glCompileShader(vertexShader);
        // check for shader compile errors
        int success;
        char infoLog[512];
        glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
            return -1;
        }
        // fragment shader
        unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
        glCompileShader(fragmentShader);
        // check for shader compile errors
        glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
            return -1;
        }
        // link shaders
        shaderProgram = glCreateProgram();
        glAttachShader(shaderProgram, vertexShader);
        glAttachShader(shaderProgram, fragmentShader);
        glLinkProgram(shaderProgram);
        // check for linking errors
        glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
            return -1;
        }
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return 1;
    }

    // Clears or Processes the screen with a given color
    void ClearScreen(float r, float g, float b, float a = 1.0f) {   
        glClearColor(r, g, b, a);
        glClear(GL_COLOR_BUFFER_BIT);
    }

    void ProcessInput() {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, true);
        }    
    }

    // Draw a triangle with given vertices
    void DrawTriangle(Triangle<float> &triangle) {
    
        triangleVerticeBuffer[verticeCount++] = triangle._p1;
        triangleVerticeBuffer[verticeCount++] = triangle._p2;
        triangleVerticeBuffer[verticeCount++] = triangle._p3;
        triangleCount++;

    }
    // Draw a rectangle with given vertices
    void DrawRectangle(Vector4<float> points) {
        
        Vector3<float> p1{points[0], points[1], 0.0f}; // Bottom-left
        Vector3<float> p2{points[0],  points[1] + points[3], 0.0f}; // Bottom-right
        Vector3<float> p3{points[0] + points[2],  points[1], 0.0f}; // Top-left
        Vector3<float> p4{points[0] + points[2],  points[1] + points[3], 0.0f}; // Top-right

        Triangle bottomTri(p1, p3, p2);
        Triangle topTri(p2, p4, p3);

        DrawTriangle(bottomTri);
        DrawTriangle(topTri);

    }

    void FlushBuffer(){
        if(triangleCount < 1){
            return;
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, triangleCount * sizeof(Triangle<float>), triangleVerticeBuffer, GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0); //unbind VBO

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, triangleCount * 3);
        glBindVertexArray(0); //unbind VAO

        triangleCount = 0;
        verticeCount = 0;
    }

    // Swaps buffers and polls events
    void EndFrame() {
        FlushBuffer();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    bool ShouldClose() {
        return glfwWindowShouldClose(window);
    }

    void Terminate() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteProgram(shaderProgram);
        glfwTerminate();
    }



private:
    GLFWwindow *window;
    int width, height, triangleCount = 0, verticeCount = 0;
    std::string title;
    GLuint VAO, VBO;
    unsigned int shaderProgram;
    Vector3<float> triangleVerticeBuffer[MAX_TRIANGLES];
    //Shader programs
    const char *vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char *fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(0.3f, 0.4f, 0.2f, 1.0f);\n"
        "}\n\0";
};
