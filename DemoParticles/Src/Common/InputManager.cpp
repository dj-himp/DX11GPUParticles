#include "pch.h"
#include "InputManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    /*InputManager::InputManager()
    {

    }

    InputManager::~InputManager()
    {
    }*/

    bool InputManager::isKeyDown(Keyboard::Keys key)
    {
        auto keyboardState = Keyboard::Get().GetState();
        return keyboardState.IsKeyDown(key);
    }

    DirectX::SimpleMath::Vector2 InputManager::getMousePosition()
    {
        auto mouseState = Mouse::Get().GetState();
        return Vector2((float)mouseState.x, (float)mouseState.y);
    }

    bool InputManager::isLeftMouseButtonDown()
    {
        auto mouseState = Mouse::Get().GetState();
        return mouseState.leftButton;
    }

    bool InputManager::isRightMouseButtonDown()
    {
        auto mouseState = Mouse::Get().GetState();
        return mouseState.rightButton;
    }

    void InputManager::update()
    {
        /*auto keyboardState = Keyboard::Get().GetState();
        m_keyboardStateTracker.Update(keyboardState);

        if (m_keyboardStateTracker.pressed.Escape)
        {
            DebugUtils::log("ESCAPE PRESSED");
        }

        if (m_keyboardStateTracker.released.Escape)
        {
            DebugUtils::log("ESCAPE RELEASED");
        }*/
    }

}
