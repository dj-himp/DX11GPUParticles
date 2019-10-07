#pragma once

#include "Common\StepTimer.h"

namespace DemoParticles
{
    class Camera;

    class IScene
    {
    public:

        IScene(const DX::DeviceResources* deviceResources);
        virtual ~IScene();

        //virtual void init();
        //virtual void release();

        virtual void createDeviceDependentResources() = 0;
        virtual void createWindowSizeDependentResources() = 0;
        virtual void releaseDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) = 0;
        virtual void render() = 0;

    protected:

        const DX::DeviceResources* m_deviceResources;

    };
}
