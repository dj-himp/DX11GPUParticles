#pragma once

#include "IRenderable.h"

#include "../Model/MeshFactory.h"
#include "ShaderStructures.h"

namespace DemoParticles
{
    class Camera;

    class MengerRenderer : public IRenderable
    {
    public:
        MengerRenderer(const DX::DeviceResources* deviceResources);

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

    private:

        std::unique_ptr<MeshFactory> m_meshFactory; //TODO probably make it as a singleton
        std::unique_ptr<Model> m_quad;

        QuadConstantBuffer   m_constantBufferData;

        DirectX::SimpleMath::Matrix m_posScale;
    };
}

