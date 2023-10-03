#include "pch.h"
#include "ParticleEmitterAppendBuffer.h"

#include "Common/ComputeShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterAppendBuffer::ParticleEmitterAppendBuffer(const DX::DeviceResources* deviceResources, std::string name)
        : IParticleEmitter(deviceResources, name, EmitterType::ET_AppendBuffer)
    {
        //m_emitterConstantBufferData.maxSpawn = 500000;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 1.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.01f;
    }

    void ParticleEmitterAppendBuffer::createDeviceDependentResources()
    {
        m_emitFromBufferParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitFromBufferParticles->load(L"EmitParticlesFromAppendBuffer_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterFromBufferConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterAppendBuffer::update(DX::StepTimer const& timer)
    {
        m_currentDelay += timer.GetElapsedSeconds();
    }

    void ParticleEmitterAppendBuffer::emit()
    {

        if (m_currentDelay < m_emitDelay)
            return;

        m_currentDelay = 0.0f;

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);
        context->CopyStructureCount(m_emitterConstantBuffer.Get(), sizeof(float) * 4, m_bufferUAV.Get());


        m_emitFromBufferParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitFromBufferParticles->setUAV(4, m_bufferUAV);
        m_emitFromBufferParticles->begin();
        m_emitFromBufferParticles->startIndirect(m_indirectArgsBuffer);
        m_emitFromBufferParticles->end();
        m_emitFromBufferParticles->setUAV(4, nullptr);
    }

    void ParticleEmitterAppendBuffer::RenderImGui(Camera* /*camera*/)
    {
        if (ImGui::TreeNode(toString().c_str()))
        {
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

    void ParticleEmitterAppendBuffer::reset()
    {
        //m_needEmit = false;
    }

    void ParticleEmitterAppendBuffer::save(json& file)
    {
        file["Type"] = m_type;
        file["Name"] = m_name;
        file["Enabled"] = m_enabled;
        file["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        //file["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
        file["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
    }

    void ParticleEmitterAppendBuffer::load(json& file)
    {
        m_enabled =                                                                         file["Enabled"];
        m_emitterConstantBufferData.maxSpawn =                                              file["Max Spawn"];
        //std::vector<float> position = file["Emitters"]["AppendBuffer"]["Position"];
        //m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation =                                   file["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed =                                    file["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan =                                     file["LifeSpan"];
        m_emitterConstantBufferData.particlesMass =                                         file["Mass"];
        std::vector<float> color =                                                          file["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart =                                     file["Size start"];
        m_emitterConstantBufferData.particleSizeEnd =                                       file["Size end"];
    }

    std::string ParticleEmitterAppendBuffer::toString()
    {
        return std::string("(AppendBuffer)") + m_name;
    }

}