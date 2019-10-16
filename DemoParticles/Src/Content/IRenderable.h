#pragma once

#include "Common\StepTimer.h"

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

        virtual void init();
        virtual void release();

        virtual void createDeviceDependentResources() = 0;
        virtual void createWindowSizeDependentResources() = 0;
        virtual void releaseDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) = 0;
        virtual void render() = 0;

    protected:

        const DX::DeviceResources* m_deviceResources;

        std::unique_ptr<Shader>     m_shader;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBuffer;
        
    };
}
