#include "pch.h"
#include "ParticleEmitterPoint.h"

#include "../Common/ComputeShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterPoint::ParticleEmitterPoint(const DX::DeviceResources* deviceResources)
        : IParticleEmitter(deviceResources)
    {
        m_emitterConstantBufferData.maxSpawn = 1000;
    }

    void ParticleEmitterPoint::createDeviceDependentResources()
    {
        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticlesPoint_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterSphereConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterPoint::update(DX::StepTimer const& timer)
    {
        if (!m_enabled)
        {
            return;
        }

        //false to reset if the previous render emit particles
        m_needEmit = false;

        m_lastEmitTime -= timer.GetElapsedSeconds();
        if (m_lastEmitTime <= 0.0)
        {
            m_lastEmitTime = m_emitDelay;
            m_needEmit = true;
        }

        //m_emitterConstantBufferData.position = DX::toVector4(camera->getPosition() + camera->getForward() * 4.0f);
        m_emitterConstantBufferData.position = Vector4(m_imGuiEmitterPosition[0], m_imGuiEmitterPosition[1], m_imGuiEmitterPosition[2], 1.0f);
        //m_emitterConstantBufferData.position = Vector4(cos(timer.GetTotalSeconds() * 0.5f) * 3.0f, 0.0f, sin(timer.GetTotalSeconds() * 0.5f) * 3.0f, 1.0f);
    }

    void ParticleEmitterPoint::emit()
    {
        if (!m_enabled || !m_needEmit)
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

    void ParticleEmitterPoint::renderImGui()
    {
        if (ImGui::TreeNode("Point emitter"))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            ImGui::DragInt("Max Spawn", (int*)&m_emitterConstantBufferData.maxSpawn, 1);
            ImGui::DragFloat3("Position", m_imGuiEmitterPosition, 0.01f);

            ImGui::TreePop();
        }
    }

}