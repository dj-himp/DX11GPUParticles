#pragma once

#include "..\Common\StepTimer.h"

namespace DemoParticles
{
    class Camera;

    class CameraControllerFPS
    {
    public:
        CameraControllerFPS(const DX::DeviceResources* deviceResources);
        ~CameraControllerFPS();

        void init(glm::vec3 position, float yaw, float pitch, float roll, float aspectRatio, float fov = 60.0f, float nearPlane = 0.1f, float farPlane = 1000.0f);
        void setAspectRatio(float aspectRatio);

        void update(DX::StepTimer const& timer);

        Camera* getCamera() { return m_camera.get();  }

    private:
        const DX::DeviceResources* m_deviceResources;

        std::unique_ptr<Camera> m_camera;

        bool m_isMouseMoveStarted = false;
        DirectX::SimpleMath::Vector2 m_previousMousePos;
    };
}
