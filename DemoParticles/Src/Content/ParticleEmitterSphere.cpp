#include "pch.h"
#include "ParticleEmitterSphere.h"

#include "../Common/ComputeShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterSphere::ParticleEmitterSphere(const DX::DeviceResources* deviceResources)
        : IParticleEmitter(deviceResources)
    {
        
    }

    void ParticleEmitterSphere::createDeviceDependentResources()
    {
        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticles_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterSphereConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterSphere::update(DX::StepTimer const& timer)
    {
        IParticleEmitter::update(timer);

        //m_emitterConstantBufferData.position = DX::toVector4(camera->getPosition() + camera->getForward() * 4.0f);
        //m_emitterConstantBufferData.position = Vector4(10.0f, 10.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.position = Vector4(cos(timer.GetTotalSeconds() * 0.5f) * 3.0f, 0.0f, sin(timer.GetTotalSeconds() * 0.5f) * 3.0f, 1.0f);
        m_emitterConstantBufferData.direction = Vector4(0.1f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.maxSpawn = 20000;
    }

    void ParticleEmitterSphere::emit()
    {
        if (!m_needEmit)
            return;

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitParticles->begin();
        m_emitParticles->start(DX::align(m_emitterConstantBufferData.maxSpawn, 1024) / 1024, 1, 1);
        m_emitParticles->end();
    }
}