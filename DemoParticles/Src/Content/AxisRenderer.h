#pragma once
#include "IRenderable.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;
    class Camera;

    class AxisRenderer : public IRenderable
    {
    public:
        AxisRenderer(const DX::DeviceResources* deviceResources);
        //~AxisRenderer();

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;

        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

    private:
        std::unique_ptr<Model> m_axis;

        DirectX::SimpleMath::Matrix m_world;

        WorldConstantBuffer   m_constantBufferData;
    };
}
