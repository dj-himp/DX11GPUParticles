#pragma once

#include "..\Common\StepTimer.h"

namespace DemoParticles
{
    class Camera;

    class CameraControllerFPS
    {
    public:
        CameraControllerFPS(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~CameraControllerFPS();

        void update(DX::StepTimer const& timer);

        Camera* getCamera() { return m_camera.get();  }

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        std::unique_ptr<Camera> m_camera;

        bool m_isMouseMoveStarted = false;
        DirectX::SimpleMath::Vector2 m_previousMousePos;
    };
}
