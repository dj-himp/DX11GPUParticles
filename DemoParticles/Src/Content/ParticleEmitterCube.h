#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

    class ParticleEmitterCube : public IParticleEmitter
    {
    public:
        ParticleEmitterCube(const DX::DeviceResources* deviceResources);
        
        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;

    private:
        
        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterCubeConstantBuffer                           m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        float m_emitDelay = 0.0f;
        float m_lastEmitTime = 0.0f;
        bool m_needEmit = false;
    };
}
