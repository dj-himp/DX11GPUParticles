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
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 1.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
    }

    void ParticleEmitterPoint::createDeviceDependentResources()
    {
        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticlesPoint_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterPointConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
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
            ImGui::DragFloat3("Position", (float*)&m_emitterConstantBufferData.position, 0.01f);
            const char* orientationItems[] = { "Billboard", "Backed Normal", "Direction" };
            ImGui::Combo("Particles orientation", (int*)&m_emitterConstantBufferData.particleOrientation, orientationItems, 3);
            ImGui::DragFloat("Base speed", &m_emitterConstantBufferData.particlesBaseSpeed, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("LifeSpan", &m_emitterConstantBufferData.particlesLifeSpan, 0.1f, -1.0f, 100.0f);
            ImGui::DragFloat("Mass", &m_emitterConstantBufferData.particlesMass, 0.1f, 0.0f, 100.0f);

            ImGui::ColorEdit4("Color", (float*)&m_emitterConstantBufferData.color);
            
            ImGui::TreePop();
        }
    }

    void ParticleEmitterPoint::save(json& file)
    {
        file["Emitters"]["Point"]["Enabled"] = m_enabled;
        file["Emitters"]["Point"]["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        file["Emitters"]["Point"]["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Emitters"]["Point"]["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Emitters"]["Point"]["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["Emitters"]["Point"]["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Emitters"]["Point"]["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Emitters"]["Point"]["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
    }

    void ParticleEmitterPoint::load(json& file)
    {
        m_enabled = file["Emitters"]["Point"]["Enabled"];
        m_emitterConstantBufferData.maxSpawn = file["Emitters"]["Point"]["Max Spawn"];
        std::vector<float> position = file["Emitters"]["Point"]["Position"];
        m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation = file["Emitters"]["Point"]["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed = file["Emitters"]["Point"]["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan = file["Emitters"]["Point"]["LifeSpan"];
        m_emitterConstantBufferData.particlesMass = file["Emitters"]["Point"]["Mass"];
        std::vector<float> color = file["Emitters"]["Point"]["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
    }

}