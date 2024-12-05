#pragma once
#define GL_SILENCE_DEPRECATION
#include <GLFW/glfw3.h>

enum KeyCode
{
    kKeyCodeUp = 0,
    kKeyCodeDown = 1,
    kKeyCodeRight = 2,
    kKeyCodeLeft = 3,
    kKeyCodeMin = kKeyCodeUp,
    kKeyCodeMax = 16,
};

enum KeyAction
{
    kKeyActionDown,
    kKeyActionUp,
};

struct KeyEvent
{
    KeyCode keyCode;
    KeyAction keyAction;
};

constexpr size_t MAX_EVENTS = 128;

/**
 * KeyEventManager implementation
 */

class KeyEventManager
{
public:
    // Public method to access the singleton instance
    inline static KeyEventManager &GetInstance()
    {
        static KeyEventManager instance; // This is the single instance
        return instance;
    }

    // Consume events
    inline void Update(uint frameCount)
    {

        if (frameCount != lastFrameCount)
        {
            // Clear last frame's events
            std::fill_n(isKeyDown, kKeyCodeMax, false);
            std::fill_n(isKeyUp, kKeyCodeMax, false);

            // Consume this frame's events
            for (size_t i = 0; i < numEvents; i++)
            {
                if (events[i].keyAction == kKeyActionDown)
                {
                    isKeyDown[events[i].keyCode] = true;
                }
                else if (events[i].keyAction == kKeyActionUp)
                {
                    isKeyUp[events[i].keyCode] = true;
                }
            }

            // Empty the queue
            numEvents = 0;
        }
        for (size_t i = 0; i < kKeyCodeMax; i++)
        {
            if (!isKeyPressed[i] && isKeyDown[i])
            {
                isKeyPressed[i] = true;
            }
            else if (isKeyPressed[i] && isKeyUp[i])
            {
                isKeyPressed[i] = false;
            }
        }

        lastFrameCount = frameCount;
    };

    // Delete copy constructor and assignment operator to prevent duplication
    KeyEventManager(const KeyEventManager &) = delete;
    KeyEventManager &operator=(const KeyEventManager &) = delete;

    // Example method
    inline void SendKeydown(KeyCode keycode)
    {
        // std::cout << "Sending keydown" << std::endl;
        events[numEvents++] = {
            .keyCode = keycode,
            .keyAction = kKeyActionDown};
    };

    inline void SendKeyup(KeyCode keycode)
    {
        events[numEvents++] = {
            .keyCode = keycode,
            .keyAction = kKeyActionUp};
    };

    inline bool Keydown(KeyCode keycode)
    {
        return isKeyDown[keycode];
    };

    inline bool Keyup(KeyCode keycode)
    {
        return isKeyUp[keycode];
    };

    inline bool Keypressed(KeyCode keycode)
    {
        return isKeyPressed[keycode];
    }

private:
    KeyEvent events[MAX_EVENTS];
    size_t numEvents = 0;

    bool isKeyDown[kKeyCodeMax];
    bool isKeyUp[kKeyCodeMax];
    bool isKeyPressed[kKeyCodeMax];

    uint lastFrameCount = 0;
    // Private constructor to prevent direct instantiation
    KeyEventManager() {}
    ~KeyEventManager() {};
};

void key_callback(GLFWwindow *, int key, int, int action, int)
{
    if (action == GLFW_PRESS)
    {
        if (key == GLFW_KEY_UP)
        {
            KeyEventManager::GetInstance().SendKeydown(kKeyCodeUp);
        }
        else if (key == GLFW_KEY_DOWN)
        {
            KeyEventManager::GetInstance().SendKeydown(kKeyCodeDown);
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            KeyEventManager::GetInstance().SendKeydown(kKeyCodeRight);
        }
        else if (key == GLFW_KEY_LEFT)
        {
            KeyEventManager::GetInstance().SendKeydown(kKeyCodeLeft);
        }
    }
    else if (action == GLFW_RELEASE)
    {
        if (key == GLFW_KEY_UP)
        {
            KeyEventManager::GetInstance().SendKeyup(kKeyCodeUp);
        }
        else if (key == GLFW_KEY_DOWN)
        {
            KeyEventManager::GetInstance().SendKeyup(kKeyCodeDown);
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            KeyEventManager::GetInstance().SendKeyup(kKeyCodeRight);
        }
        else if (key == GLFW_KEY_LEFT)
        {
            KeyEventManager::GetInstance().SendKeyup(kKeyCodeLeft);
        }
    }
}