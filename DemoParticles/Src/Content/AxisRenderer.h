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
        AxisRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~AxisRenderer();

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;

        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

    private:
        std::unique_ptr<Model> m_axis;
        std::unique_ptr<Camera> m_camera;

        DirectX::SimpleMath::Matrix m_world;

        ModelConstantBuffer   m_constantBufferData;
    };
}
