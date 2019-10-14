#pragma once

#include "IRenderable.h"

#include "Content/ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

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

        void resetParticles();
        void emitParticles();
        void simulateParticles();

        struct Particle
        {
            DirectX::SimpleMath::Vector4 position;
        };

        struct ParticleIndexElement
        {
            float distance; //squared distance from camera
            float index; //index in the particle buffer
        };

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
        int m_maxParticles = 10000;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_particleBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleSRV;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_particleUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_deadListUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveIndexBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_aliveIndexSRV;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAV;

        std::unique_ptr<ComputeShader>                      m_initDeadListShader;
        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterConstantBuffer                               m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        DeadListCountConstantBuffer                         m_deadListCountConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDrawArgsBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDrawArgsUAV;

        std::unique_ptr<ComputeShader>                      m_simulateShader;

        bool m_resetParticles = true;
    };
}
