#include "pch.h"
#include "ParticleEmitterBuffer.h"

#include "Common/ComputeShader.h"

namespace DemoParticles
{
    ParticleEmitterBuffer::ParticleEmitterBuffer(const DX::DeviceResources* deviceResources)
        : IParticleEmitter(deviceResources)
    {
    }

    void ParticleEmitterBuffer::createDeviceDependentResources()
    {
        m_emitFromBufferParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitFromBufferParticles->load(L"EmitParticlesFromBuffer_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterFromBufferConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterBuffer::update(DX::StepTimer const& timer)
    {
        IParticleEmitter::update(timer);
    }

    void ParticleEmitterBuffer::emit()
    {
        if (m_bufferUAV == nullptr)
        {
            assert(0);
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        //int i = m_initDeadListShader->readCounter(m_deadListUAV);
        //DebugUtils::log(std::to_string(i));

        //update the number of baked particle (TO DO do it once if no changes)
        context->CopyStructureCount(m_emitterConstantBuffer.Get(), 0, m_bufferUAV.Get());

        UINT initialCount[] = { -1 };
        m_emitFromBufferParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitFromBufferParticles->setUAV(2, m_bufferUAV);
        m_emitFromBufferParticles->begin();
        m_emitFromBufferParticles->startIndirect(m_indirectArgsBuffer);
        m_emitFromBufferParticles->end();
        m_emitFromBufferParticles->setUAV(0, nullptr);
        m_emitFromBufferParticles->setUAV(1, nullptr);
        m_emitFromBufferParticles->setUAV(2, nullptr);
        
    }
}