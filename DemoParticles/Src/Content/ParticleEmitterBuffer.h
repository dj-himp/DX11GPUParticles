#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

    class ParticleEmitterBuffer : public IParticleEmitter
    {
    public:
        ParticleEmitterBuffer(const DX::DeviceResources* deviceResources);

        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;

        void setBuffer(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> bufferUAV) { m_bufferUAV = bufferUAV; }

        //TODO : maybe set that in this class
        void setIndirectArgsBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indirectArgsBuffer) { m_indirectArgsBuffer = indirectArgsBuffer; }

    private:

        std::unique_ptr<ComputeShader>                      m_emitFromBufferParticles;

        EmitterFromBufferConstantBuffer                     m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_bufferUAV;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectArgsBuffer;

        bool m_hasEmitted = false;
    };
}