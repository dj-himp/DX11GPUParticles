#include "pch.h"
#include "CameraControllerFPS.h"

#include "Camera.h"
#include "Common/InputManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    CameraControllerFPS::CameraControllerFPS(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {
        float yaw = 0.0f;
        float pitch = 0.0f;
        init(glm::vec3(-2.0f, 2.0f, 5.0f), yaw, pitch, 0.0f, (float)m_deviceResources->GetOutputWidth() / m_deviceResources->GetOutputHeight(), DirectX::XM_PI / 4.0f);
    }

    //needed to use forward declaration of a unique_ptr : 
    //default destructor is inline so the unique_ptr type is incomplete in the header
    CameraControllerFPS::~CameraControllerFPS()
    {
    }

    void CameraControllerFPS::init(glm::vec3 position, float yaw, float pitch, float roll, float aspectRatio, float fov /*= 60.0f*/, float nearPlane /*= 0.1f*/, float farPlane /*= 1000.0f*/)
    {
        m_camera = std::make_unique<Camera>(position, yaw, pitch, roll, aspectRatio, fov, nearPlane, farPlane);
    }

    void CameraControllerFPS::setAspectRatio(float aspectRatio)
    {
        m_camera->setAspectRatio(aspectRatio);
    }

    void DemoParticles::CameraControllerFPS::update(DX::StepTimer const& timer)
    {
        float m_movementSpeed = 5.0f;
        if (InputManager::isKeyDown(Keyboard::LeftShift))
        {
            m_movementSpeed *= 2.0f;
        }

        glm::vec3 movement(0.0f);

        bool hasMoved = false;
        if (InputManager::isKeyDown(Keyboard::Z))
        {
            movement.z += m_movementSpeed * (float)timer.GetElapsedSeconds();
            hasMoved = true;
        }
        if (InputManager::isKeyDown(Keyboard::S))
        {
            movement.z -= m_movementSpeed * (float)timer.GetElapsedSeconds();
            hasMoved = true;
        }
        if (InputManager::isKeyDown(Keyboard::D))
        {
            movement.x += m_movementSpeed * (float)timer.GetElapsedSeconds();
            hasMoved = true;
        }
        if (InputManager::isKeyDown(Keyboard::Q))
        {
            movement.x -= m_movementSpeed * (float)timer.GetElapsedSeconds();
            hasMoved = true;
        }
        if (InputManager::isKeyDown(Keyboard::Space))
        {
            movement.y += m_movementSpeed * (float)timer.GetElapsedSeconds();
            hasMoved = true;
        }
        if (InputManager::isKeyDown(Keyboard::LeftControl))
        {
            movement.y -= m_movementSpeed * (float)timer.GetElapsedSeconds();
            hasMoved = true;
        }

        if (hasMoved)
        {
            m_camera->movePositionRelative(movement);
        }

        if (InputManager::isRightMouseButtonDown())
        {
            Vector2 mousePosition = InputManager::getMousePosition();
            if (!m_isMouseMoveStarted)
            {
                m_isMouseMoveStarted = true;
                m_previousMousePos = mousePosition;
            }
            else
            {
                float m_mouseSensibility = 0.01f;
                Vector2 delta = (mousePosition - m_previousMousePos) * m_mouseSensibility;
                m_camera->rotate(delta.x, delta.y);
                
                m_previousMousePos = mousePosition;
            }
        }
        else
        {
            m_isMouseMoveStarted = false;
        }
    }
}