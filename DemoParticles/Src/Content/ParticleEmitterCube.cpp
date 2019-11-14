#include "pch.h"
#include "ParticleEmitterCube.h"

#include "../Common/ComputeShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterCube::ParticleEmitterCube(const DX::DeviceResources* deviceResources)
        : IParticleEmitter(deviceResources)
    {
        m_emitDelay = 0.1f;
    }

    void ParticleEmitterCube::createDeviceDependentResources()
    {
        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticlesCube_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterCubeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterCube::update(DX::StepTimer const& timer)
    {
        //false to reset if the previous render emit particles
        m_needEmit = false;

        m_lastEmitTime -= timer.GetElapsedSeconds();
        if (m_lastEmitTime <= 0.0)
        {
            m_lastEmitTime = m_emitDelay;
            m_needEmit = true;
        }

        m_emitterConstantBufferData.world = Matrix::CreateScale(8.0f, 8.0f, 8.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationX(0.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationY(0.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationZ(0.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
        m_emitterConstantBufferData.maxSpawn = 200000;
    }

    void ParticleEmitterCube::emit()
    {
        if (!m_needEmit)
        {
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitParticles->begin();
        m_emitParticles->start(DX::align(m_emitterConstantBufferData.maxSpawn, 1024) / 1024, 1, 1);
        m_emitParticles->end();
    }
}