#include "pch.h"
#include "ParticleEmitterBuffer.h"

#include "Common/ComputeShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterBuffer::ParticleEmitterBuffer(const DX::DeviceResources* deviceResources)
        : IParticleEmitter(deviceResources)
    {
        m_emitterConstantBufferData.maxSpawn = 500000;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 0.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.01f;
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

    void ParticleEmitterBuffer::update(DX::StepTimer const& /*timer*/)
    {

    }

    void ParticleEmitterBuffer::emit()
    {

        if (!m_needEmit)
        {
            return;
        }

        if (m_bufferUAV == nullptr)
        {
            assert(0);
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        //int i = m_emitFromBufferParticles->readCounter(m_bufferUAV);
        //DebugUtils::log(std::to_string(i));

        //update the number of baked particle (TO DO do it once if no changes)
        context->CopyStructureCount(m_emitterConstantBuffer.Get(), 0, m_bufferUAV.Get());

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitFromBufferParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitFromBufferParticles->setUAV(2, m_bufferUAV);
        m_emitFromBufferParticles->begin();
        m_emitFromBufferParticles->startIndirect(m_indirectArgsBuffer);
        m_emitFromBufferParticles->end();
        m_emitFromBufferParticles->setUAV(2, nullptr);
        

        m_needEmit = false;
    }

    void ParticleEmitterBuffer::RenderImGui(Camera* /*camera*/)
    {
        if (ImGui::TreeNode("Buffer emitter"))
        {
            if (ImGui::Button("Spawn"))
            {
                m_needEmit = true;
            }
            ImGui::DragInt("Max Spawn", (int*)&m_emitterConstantBufferData.maxSpawn, 1, 0, 10000000);
            //ImGui::DragFloat3("Position", (float*)&m_emitterConstantBufferData.position, 0.01f);
            const char* orientationItems[] = { "Billboard", "Backed Normal", "Direction" };
            ImGui::Combo("Particles orientation", (int*)&m_emitterConstantBufferData.particleOrientation, orientationItems, 3);
            ImGui::DragFloat("Base speed", &m_emitterConstantBufferData.particlesBaseSpeed, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("LifeSpan", &m_emitterConstantBufferData.particlesLifeSpan, 0.1f, -1.0f, 100.0f);
            ImGui::DragFloat("Mass", &m_emitterConstantBufferData.particlesMass, 0.1f, 0.0f, 100.0f);
            ImGui::ColorEdit4("Color", (float*)&m_emitterConstantBufferData.color);
            ImGui::DragFloat("Size Start", &m_emitterConstantBufferData.particleSizeStart, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Size End", &m_emitterConstantBufferData.particleSizeEnd, 0.01f, 0.0f, 10.0f);

            ImGui::TreePop();
        }
    }

    void ParticleEmitterBuffer::reset()
    {
        m_needEmit = false;
    }

    void ParticleEmitterBuffer::save(json& file)
    {
        file["Emitters"]["Buffer"]["Enabled"] = m_enabled;
        file["Emitters"]["Buffer"]["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        //file["Emitters"]["Buffer"]["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Emitters"]["Buffer"]["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Emitters"]["Buffer"]["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["Emitters"]["Buffer"]["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Emitters"]["Buffer"]["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Emitters"]["Buffer"]["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
        file["Emitters"]["Buffer"]["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Emitters"]["Buffer"]["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
    }

    void ParticleEmitterBuffer::load(json& file)
    {
        m_enabled = file["Emitters"]["Buffer"]["Enabled"];
        m_emitterConstantBufferData.maxSpawn = file["Emitters"]["Buffer"]["Max Spawn"];
        //std::vector<float> position = file["Emitters"]["Buffer"]["Position"];
        //m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation = file["Emitters"]["Buffer"]["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed = file["Emitters"]["Buffer"]["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan = file["Emitters"]["Buffer"]["LifeSpan"];
        m_emitterConstantBufferData.particlesMass = file["Emitters"]["Buffer"]["Mass"];
        std::vector<float> color = file["Emitters"]["Buffer"]["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart = file["Emitters"]["Buffer"]["Size start"];
        m_emitterConstantBufferData.particleSizeEnd = file["Emitters"]["Buffer"]["Size end"];
    }

}