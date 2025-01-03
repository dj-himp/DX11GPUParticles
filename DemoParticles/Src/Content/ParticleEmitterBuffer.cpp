#include "pch.h"
#include "ParticleEmitterBuffer.h"

#include "Common/ComputeShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterBuffer::ParticleEmitterBuffer(const DX::DeviceResources* deviceResources, std::string name)
        : IParticleEmitter(deviceResources, name, EmitterType::ET_Buffer)
    {
        m_emitterConstantBufferData.maxSpawn = 500000;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 0.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.colorStart = Color(0.5f, 0.2f, 0.2f, 1.0f);
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
        //PROBABLY WRONG SINCE IT WILL ERASE CpoyStructureCount
        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitFromBufferParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitFromBufferParticles->setUAV(4, m_bufferUAV);
        m_emitFromBufferParticles->begin();
        m_emitFromBufferParticles->startIndirect(m_indirectArgsBuffer);
        m_emitFromBufferParticles->end();
        m_emitFromBufferParticles->setUAV(4, nullptr);
        

        m_needEmit = false;
    }

    void ParticleEmitterBuffer::RenderImGui(Camera* /*camera*/)
    {
        if (ImGui::TreeNode(toString().c_str()))
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
            ImGui::ColorEdit4("Color Start", (float*)&m_emitterConstantBufferData.colorStart);
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
        file["Type"] = m_type;
        file["Name"] = m_name;
        file["Enabled"] = m_enabled;
        file["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        //file["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Color"] = { m_emitterConstantBufferData.colorStart.R(), m_emitterConstantBufferData.colorStart.G(), m_emitterConstantBufferData.colorStart.B(), m_emitterConstantBufferData.colorStart.A() };
        file["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
    }

    void ParticleEmitterBuffer::load(json& file)
    {
        m_enabled =                                                                     file["Enabled"];
        m_emitterConstantBufferData.maxSpawn =                                          file["Max Spawn"];
        //std::vector<float> position = file["Emitters"]["Buffer"]["Position"];
        //m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation =                               file["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed =                                file["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan =                                 file["LifeSpan"];
        m_emitterConstantBufferData.particlesMass =                                     file["Mass"];
        std::vector<float> color =                                                      file["Color"];
        m_emitterConstantBufferData.colorStart = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart =                                 file["Size start"];
        m_emitterConstantBufferData.particleSizeEnd =                                   file["Size end"];
    }

    std::string ParticleEmitterBuffer::toString()
    {
        return std::string("(Buffer)") + m_name;
    }

}