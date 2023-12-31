#pragma once

#include "IRenderable.h"

#include "../Model/MeshFactory.h"
#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;
    class Camera;
    class VertexShader;
    class PixelShader;

    class RenderFullscreenQuad : public IRenderable
    {
    public:
        RenderFullscreenQuad(const DX::DeviceResources* deviceResources);

        void createDeviceDependentResources() override;
        void createWindowSizeDependentResources() override;
        void releaseDeviceDependentResources() override;

        void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        void render() override;

        void setPosScale(const DirectX::SimpleMath::Matrix posScale);
        void setTexture(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture);

    private:

        std::unique_ptr<Model> m_quad;
        std::unique_ptr<VertexShader> m_quadVS;
        std::unique_ptr<PixelShader> m_quadPS;

        QuadConstantBuffer   m_constantBufferData;

        DirectX::SimpleMath::Matrix m_posScale;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_texture;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pointSamplerState;
    };
}
