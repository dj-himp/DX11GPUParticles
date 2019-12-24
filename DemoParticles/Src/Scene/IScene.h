#pragma once

#include "Common\StepTimer.h"

using json = nlohmann::json;

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

        virtual void RenderImGui(Camera* camera) { (void)camera; }
        virtual void save(json& file) { (void)file; }
        virtual void load(json& file) { (void)file; }

    protected:

        const DX::DeviceResources* m_deviceResources;

    };
}
