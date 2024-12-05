#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

    class ParticleEmitterAppendBuffer : public IParticleEmitter
    {
    public:
        ParticleEmitterAppendBuffer(const DX::DeviceResources* deviceResources, std::string names);

        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;
        virtual void RenderImGui(Camera* camera) override;
        virtual void reset() override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;
        virtual std::string toString() override;

        void setAppendBuffer(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> bufferUAV) { m_bufferUAV = bufferUAV; }
        
        //TODO : maybe set that in this class
        void setIndirectArgsBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indirectArgsBuffer) { m_indirectArgsBuffer = indirectArgsBuffer; }

    private:

        std::unique_ptr<ComputeShader>                      m_emitFromBufferParticles;

        EmitterFromBufferConstantBuffer                     m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_bufferUAV;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectArgsBuffer;

        float m_emitDelay = 1.0f / 60.0f;
        float m_currentDelay = 0.0f;
        
        Microsoft::WRL::ComPtr<ID3D11Resource>              m_particleTexture1;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleTexture1SRV;
        int m_uvTileX = 0;
        int m_uvTileY = 0;
    };
}
