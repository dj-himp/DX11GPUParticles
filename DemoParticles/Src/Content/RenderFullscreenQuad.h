#pragma once

#include "IRenderable.h"

#include "../Model/QuadCreator.h"
#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;
    class Camera;

    class RenderFullscreenQuad : public IRenderable
    {
    public:
        RenderFullscreenQuad(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void init() override;
        void release() override;

        void createDeviceDependentResources() override;
        void createWindowSizeDependentResources() override;
        void releaseDeviceDependentResources() override;

        void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        void render() override;        

    private:

        std::unique_ptr<QuadCreator> m_quadCreator; //TODO probably make it as a singleton
        std::unique_ptr<Model> m_quad;

        QuadConstantBuffer   m_constantBufferData;

        DirectX::SimpleMath::Matrix m_posScale;
    };
}
