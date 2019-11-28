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
        m_emitterConstantBufferData.maxSpawn = 200000;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 0.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.01f;
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

        m_emitterConstantBufferData.world = Matrix::CreateScale(m_cubeSize);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationX(0.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationY(0.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationZ(0.0f);
        m_emitterConstantBufferData.world *= Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
        m_emitterConstantBufferData.world = m_emitterConstantBufferData.world.Transpose();
        
    }

    void ParticleEmitterCube::emit()
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

    void ParticleEmitterCube::renderImGui()
    {
        if (ImGui::TreeNode("Cube emitter"))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
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

    void ParticleEmitterCube::save(json& file)
    {
        file["Emitters"]["Cube"]["Enabled"] = m_enabled;
        file["Emitters"]["Cube"]["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        //file["Emitters"]["Cube"]["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Emitters"]["Cube"]["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Emitters"]["Cube"]["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["Emitters"]["Cube"]["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Emitters"]["Cube"]["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Emitters"]["Cube"]["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
        file["Emitters"]["Cube"]["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Emitters"]["Cube"]["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
    }

    void ParticleEmitterCube::load(json& file)
    {
        m_enabled = file["Emitters"]["Cube"]["Enabled"];
        m_emitterConstantBufferData.maxSpawn = file["Emitters"]["Cube"]["Max Spawn"];
        //std::vector<float> position = file["Emitters"]["Cube"]["Position"];
        //m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation = file["Emitters"]["Cube"]["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed = file["Emitters"]["Cube"]["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan = file["Emitters"]["Cube"]["LifeSpan"];
        m_emitterConstantBufferData.particlesMass = file["Emitters"]["Cube"]["Mass"];
        std::vector<float> color = file["Emitters"]["Cube"]["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart = file["Emitters"]["Cube"]["Size start"];
        m_emitterConstantBufferData.particleSizeEnd = file["Emitters"]["Cube"]["Size end"];
    }

}