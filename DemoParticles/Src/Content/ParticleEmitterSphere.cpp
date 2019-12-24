#include "pch.h"
#include "ParticleEmitterSphere.h"

#include "../Common/ComputeShader.h"
#include "../Camera/Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterSphere::ParticleEmitterSphere(const DX::DeviceResources* deviceResources)
        : IParticleEmitter(deviceResources)
    {
        m_emitterConstantBufferData.maxSpawn = 1000;
        m_emitterConstantBufferData.position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.scale = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 1.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.0f;
    }

    void ParticleEmitterSphere::createDeviceDependentResources()
    {
        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticlesSphere_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterSphereConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterSphere::update(DX::StepTimer const& timer)
    {
        if (!m_enabled)
        {
            return;
        }

        //false to reset if the previous render emit particles
        m_needEmit = false;

        m_lastEmitTime -= (float)timer.GetElapsedSeconds();
        if (m_lastEmitTime <= 0.0)
        {
            m_lastEmitTime = m_emitDelay;
            m_needEmit = true;
        }
    }

    void ParticleEmitterSphere::emit()
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

    void ParticleEmitterSphere::RenderImGui(Camera* camera)
    {
        if (ImGui::TreeNode("Sphere emitter"))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            ImGui::DragInt("Max Spawn", (int*)&m_emitterConstantBufferData.maxSpawn, 1, 0, 10000000);
            const char* orientationItems[] = { "Billboard", "Backed Normal", "Direction" };
            ImGui::Combo("Particles orientation", (int*)&m_emitterConstantBufferData.particleOrientation, orientationItems, 3);
            ImGui::DragFloat("Base speed", &m_emitterConstantBufferData.particlesBaseSpeed, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("LifeSpan", &m_emitterConstantBufferData.particlesLifeSpan, 0.1f, -1.0f, 100.0f);
            ImGui::DragFloat("Mass", &m_emitterConstantBufferData.particlesMass, 0.1f, 0.0f, 100.0f);
            ImGui::ColorEdit4("Color", (float*)&m_emitterConstantBufferData.color);
            ImGui::DragFloat("Size Start", &m_emitterConstantBufferData.particleSizeStart, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Size End", &m_emitterConstantBufferData.particleSizeEnd, 0.01f, 0.0f, 10.0f);

            float rotation[3];
            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.scale);
            ImGui::DragFloat3("Position", (float*)&m_emitterConstantBufferData.position, 0.01f);
            ImGui::DragFloat3("Scale", (float*)&m_emitterConstantBufferData.scale, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.scale, m_worldf);

            float p[3];
            float r[3];
            ImGuizmo::DecomposeMatrixToComponents(m_fakeWorldf, p, r, (float*)&m_emitterConstantBufferData.partitioning);
            ImGui::DragFloat3("Partitioning", (float*)&m_emitterConstantBufferData.partitioning, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.partitioning, m_fakeWorldf);

            static bool guizmoHidden = true;
            if (!m_enabled)
            {
                guizmoHidden = true;
            }

            static bool useScaleForPartitioning;

            if (ImGui::RadioButton("None", guizmoHidden))
            {
                guizmoHidden = true;
            }
            ImGui::SameLine();

            static float snap[3] = { 0.1f, 0.1f, 0.1f };
            static ImGuizmo::OPERATION guizmoOperation = ImGuizmo::TRANSLATE;
            static ImGuizmo::MODE guizmoMode = ImGuizmo::WORLD;
            if (ImGui::RadioButton("Translate", guizmoOperation == ImGuizmo::TRANSLATE && useScaleForPartitioning == false && guizmoHidden == false))
            {
                guizmoOperation = ImGuizmo::TRANSLATE;
                useScaleForPartitioning = false;
                guizmoHidden = false;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", guizmoOperation == ImGuizmo::SCALE && useScaleForPartitioning == false && guizmoHidden == false))
            {
                guizmoOperation = ImGuizmo::SCALE;
                useScaleForPartitioning = false;
                guizmoHidden = false;
            }
            
            ImGui::SameLine();
            if (ImGui::RadioButton("Partitioning", useScaleForPartitioning == true && guizmoHidden == false))
            {
                guizmoOperation = ImGuizmo::SCALE;
                useScaleForPartitioning = true;
                guizmoHidden = false;
            }

            if (guizmoHidden == false)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                if (!useScaleForPartitioning)
                {
                    ImGuizmo::Manipulate(&camera->getView().Transpose().m[0][0], &camera->getProjection().Transpose().m[0][0], guizmoOperation, guizmoMode, m_worldf, nullptr, /*snap*/nullptr);
                    ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.scale);
                }
                else
                {
                    ImGuizmo::Manipulate(&camera->getView().Transpose().m[0][0], &camera->getProjection().Transpose().m[0][0], guizmoOperation, guizmoMode, m_fakeWorldf, nullptr, nullptr);
                    ImGuizmo::DecomposeMatrixToComponentsRadians(m_fakeWorldf, p, r, (float*)&m_emitterConstantBufferData.partitioning);
                }
            }

            ImGui::TreePop();
        }

        //ImGuizmo::
    }

    void ParticleEmitterSphere::save(json& file)
    {
        file["Emitters"]["Sphere"]["Enabled"] = m_enabled;
        file["Emitters"]["Sphere"]["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        file["Emitters"]["Sphere"]["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Emitters"]["Sphere"]["Scale"] = { m_emitterConstantBufferData.scale.x, m_emitterConstantBufferData.scale.y, m_emitterConstantBufferData.scale.z, m_emitterConstantBufferData.scale.w };
        file["Emitters"]["Sphere"]["Partitioning"] = { m_emitterConstantBufferData.partitioning.x, m_emitterConstantBufferData.partitioning.y, m_emitterConstantBufferData.partitioning.z, m_emitterConstantBufferData.partitioning.w };
        file["Emitters"]["Sphere"]["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Emitters"]["Sphere"]["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["Emitters"]["Sphere"]["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Emitters"]["Sphere"]["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Emitters"]["Sphere"]["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
        file["Emitters"]["Sphere"]["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Emitters"]["Sphere"]["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
    }

    void ParticleEmitterSphere::load(json& file)
    {
        m_enabled = file["Emitters"]["Sphere"]["Enabled"];
        m_emitterConstantBufferData.maxSpawn = file["Emitters"]["Sphere"]["Max Spawn"];
        std::vector<float> position = file["Emitters"]["Sphere"]["Position"];
        m_emitterConstantBufferData.position = Vector4(&position[0]);
        std::vector<float> scale = file["Emitters"]["Sphere"]["Scale"];
        m_emitterConstantBufferData.scale = Vector4(&scale[0]);
        std::vector<float> partitioning = file["Emitters"]["Sphere"]["Partitioning"];
        m_emitterConstantBufferData.partitioning = Vector4(&partitioning[0]);
        m_emitterConstantBufferData.particleOrientation = file["Emitters"]["Sphere"]["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed = file["Emitters"]["Sphere"]["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan = file["Emitters"]["Sphere"]["LifeSpan"];
        m_emitterConstantBufferData.particlesMass = file["Emitters"]["Sphere"]["Mass"];
        std::vector<float> color = file["Emitters"]["Sphere"]["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart = file["Emitters"]["Sphere"]["Size start"];
        m_emitterConstantBufferData.particleSizeEnd = file["Emitters"]["Sphere"]["Size end"];

        float rotation[3] = { 0.0f, 0.0f, 0.0f };
        ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.scale, m_worldf);
        
        ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.partitioning, m_fakeWorldf);
    }

}