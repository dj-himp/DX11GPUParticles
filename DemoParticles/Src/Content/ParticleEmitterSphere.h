#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

    class ParticleEmitterSphere : public IParticleEmitter
    {
    public:
        ParticleEmitterSphere(const DX::DeviceResources* deviceResources);
        
        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;

    private:
        
        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterSphereConstantBuffer                         m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;
    };
}
