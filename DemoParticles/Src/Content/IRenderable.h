#pragma once

#include "Common\StepTimer.h"

using json = nlohmann::json;

namespace DemoParticles
{
    class Camera;
    class Shader;
    class Model;

    class IRenderable
    {
    public:

        IRenderable(const DX::DeviceResources* deviceResources);
        virtual ~IRenderable();

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

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBuffer;
        
    };
}
