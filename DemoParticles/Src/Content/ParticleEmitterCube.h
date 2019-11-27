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
        virtual void renderImGui() override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;

        void setCubeSize(DirectX::SimpleMath::Vector3 cubeSize) { m_cubeSize = cubeSize; }

    private:
        
        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterCubeConstantBuffer                           m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        float m_emitDelay = 0.0f;
        float m_lastEmitTime = 0.0f;
        bool m_needEmit = false;

        DirectX::SimpleMath::Vector3 m_cubeSize = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
    };
}
