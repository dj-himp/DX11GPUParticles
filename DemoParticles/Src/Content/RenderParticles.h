#pragma once

#include "IRenderable.h"

#include "Content/ShaderStructures.h"

namespace DemoParticles
{
    class Model;

    class RenderParticles : public IRenderable
    {
    public:
        RenderParticles(const DX::DeviceResources* deviceResources);

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

        void setShaderResourceViews(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionView, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalView);
    private:
        std::unique_ptr<Shader>     m_moveShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_vertexBuffer;
        UINT                                    m_vertexStride;

        WorldConstantBuffer                 m_constantBufferDataVS;
        ModelViewProjectionConstantBuffer   m_constantBufferDataGS;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pointSamplerState;

        DirectX::SimpleMath::Matrix     m_world;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_positionView;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalView;

        int m_nbParticles;
    };
}
