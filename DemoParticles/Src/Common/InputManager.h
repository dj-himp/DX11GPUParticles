#pragma once

//TEMPORARY Input management 
//Need to create a proper manager

namespace DemoParticles
{
    static class InputManager
    {
    public:
        //InputManager();
        //~InputManager();

        static bool isKeyDown(DirectX::Keyboard::Keys key);
        static DirectX::SimpleMath::Vector2 getMousePosition();
        static bool isLeftMouseButtonDown();
        static bool isRightMouseButtonDown();
        static void update();


    private:

        DirectX::Keyboard::KeyboardStateTracker m_keyboardStateTracker;
    };
}
