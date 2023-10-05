#include "pch.h"
#include "ParticleEmitterSphere.h"

#include "../Common/ComputeShader.h"
#include "../Camera/Camera.h"

using namespace DirectX::SimpleMath;
using namespace choreograph;

namespace DemoParticles
{
    ParticleEmitterSphere::ParticleEmitterSphere(const DX::DeviceResources* deviceResources, std::string name)
        : IParticleEmitter(deviceResources, name, EmitterType::ET_Sphere)
    {
        m_emitterConstantBufferData.maxSpawn = 1000;
        m_emitterConstantBufferData.position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.scale = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        m_emitterConstantBufferData.partitioning = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        m_emitterConstantBufferData.rotation = Matrix::CreateRotationX(m_rotation[0]) * Matrix::CreateRotationY(m_rotation[1]) * Matrix::CreateRotationZ(m_rotation[2]);
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

        if (m_emissionRate > 0.0f)
        {
            m_emissionRateAccumulation += m_emissionRate * timer.GetElapsedSeconds();

            if (m_emissionRateAccumulation > 1.0f)
            {
                float integerPart = 0.0f;
                float fraction = modf(m_emissionRateAccumulation, &integerPart);
                m_emitterConstantBufferData.maxSpawn = integerPart;
                m_emissionRateAccumulation = fraction;
            }
            else
            {
                m_emitterConstantBufferData.maxSpawn = 0;
            }
        }
        else
        {
            m_emitterConstantBufferData.maxSpawn = 0;
            m_emissionRateAccumulation = 0;
        }

        //m_timeline.step(timer.GetElapsedSeconds());
        //m_emitterConstantBufferData.position = m_ChoreoPosition;

    }

    void ParticleEmitterSphere::emit()
    {
        if (!m_enabled || m_emitterConstantBufferData.maxSpawn == 0)
        {
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitParticles->begin();
        // div by 256 == "WARP_SIZE" (same in CS shader)
        //maxSpawn / 256 as group max so and in shader it's 256 so we spawn maxspawn aligned to 256 threads
        m_emitParticles->start(DX::align(m_emitterConstantBufferData.maxSpawn, 256) / 256, 1, 1);
        m_emitParticles->end();
    }

    void ParticleEmitterSphere::RenderImGui(Camera* camera)
    {
        if (ImGui::TreeNode(toString().c_str()))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            //ImGui::DragInt("Max Spawn", (int*)&m_emitterConstantBufferData.maxSpawn, 1, 0, 10000000);
            ImGui::DragFloat("Emission Rate", (float*)&m_emissionRate, 1.0f, 0.0f, 10000000.0f);
            const char* orientationItems[] = { "Billboard", "Backed Normal", "Direction" };
            ImGui::Combo("Particles orientation", (int*)&m_emitterConstantBufferData.particleOrientation, orientationItems, 3);
            ImGui::DragFloat("Base speed", &m_emitterConstantBufferData.particlesBaseSpeed, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("LifeSpan", &m_emitterConstantBufferData.particlesLifeSpan, 0.1f, -1.0f, 100.0f);
            ImGui::DragFloat("Mass", &m_emitterConstantBufferData.particlesMass, 0.1f, 0.0f, 100.0f);
            ImGui::ColorEdit4("Color", (float*)&m_emitterConstantBufferData.color);
            ImGui::DragFloat("Size Start", &m_emitterConstantBufferData.particleSizeStart, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Size End", &m_emitterConstantBufferData.particleSizeEnd, 0.01f, 0.0f, 10.0f);

            
            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_emitterConstantBufferData.position, (float*)&m_rotation[0], (float*)&m_emitterConstantBufferData.scale);
            ImGui::DragFloat3("Position", (float*)&m_emitterConstantBufferData.position, 0.01f);
            ImGui::DragFloat3("Scale", (float*)&m_emitterConstantBufferData.scale, 0.01f);
            if (ImGui::DragFloat3("Rotation", &m_rotation[0], 0.01f))
            {
                m_emitterConstantBufferData.rotation = Matrix::CreateRotationX(m_rotation[0]) * Matrix::CreateRotationY(m_rotation[1]) * Matrix::CreateRotationZ(m_rotation[2]);
                m_emitterConstantBufferData.rotation = m_emitterConstantBufferData.rotation.Transpose();
            }
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&m_rotation[0], (float*)&m_emitterConstantBufferData.scale, m_worldf);

            float p[3];
            float r[3];
            ImGuizmo::DecomposeMatrixToComponents(m_fakeWorldf, p, r, (float*)&m_emitterConstantBufferData.partitioning);
            ImGui::DragFloat3("Partitioning", (float*)&m_emitterConstantBufferData.partitioning, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&r, (float*)&m_emitterConstantBufferData.partitioning, m_fakeWorldf);

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
            if (ImGui::RadioButton("Translate", m_guizmoOperation == ImGuizmo::TRANSLATE && useScaleForPartitioning == false && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::TRANSLATE;
                useScaleForPartitioning = false;
                guizmoHidden = false;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_guizmoOperation == ImGuizmo::SCALE && useScaleForPartitioning == false && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::SCALE;
                useScaleForPartitioning = false;
                guizmoHidden = false;
            }
            
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotation", m_guizmoOperation == ImGuizmo::ROTATE && useScaleForPartitioning == false && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::ROTATE;
                useScaleForPartitioning = false;
                guizmoHidden = false;
            }

            ImGui::SameLine();
            if (ImGui::RadioButton("Partitioning", useScaleForPartitioning == true && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::SCALE;
                useScaleForPartitioning = true;
                guizmoHidden = false;
            }

            if (guizmoHidden == false)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                if (!useScaleForPartitioning)
                {
                    ImGuizmo::Manipulate(&camera->getView()[0][0], &camera->getProjection()[0][0], m_guizmoOperation, m_guizmoMode, m_worldf, nullptr, /*snap*/nullptr);
                    ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_emitterConstantBufferData.position, (float*)&m_rotation[0], (float*)&m_emitterConstantBufferData.scale);
                    m_emitterConstantBufferData.rotation = Matrix::CreateRotationX(m_rotation[0]) * Matrix::CreateRotationY(m_rotation[1]) * Matrix::CreateRotationZ(m_rotation[2]);
                    m_emitterConstantBufferData.rotation = m_emitterConstantBufferData.rotation.Transpose();
                }
                else
                {
                    ImGuizmo::Manipulate(&camera->getView()[0][0], &camera->getProjection()[0][0], m_guizmoOperation, m_guizmoMode, m_fakeWorldf, nullptr, nullptr);
                    ImGuizmo::DecomposeMatrixToComponentsRadians(m_fakeWorldf, p, r, (float*)&m_emitterConstantBufferData.partitioning);
                }
            }

            ImGui::TreePop();
        }
    }

    void ParticleEmitterSphere::save(json& file)
    {
        file["Type"] = m_type;
        file["Name"] = m_name;
        file["Enabled"] = m_enabled;
        file["EmissionRate"] = m_emissionRate;
        file["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Scale"] = { m_emitterConstantBufferData.scale.x, m_emitterConstantBufferData.scale.y, m_emitterConstantBufferData.scale.z, m_emitterConstantBufferData.scale.w };
        file["Rotation"] = { m_rotation[0], m_rotation[1], m_rotation[2] };
        file["Partitioning"] = { m_emitterConstantBufferData.partitioning.x, m_emitterConstantBufferData.partitioning.y, m_emitterConstantBufferData.partitioning.z, m_emitterConstantBufferData.partitioning.w };
        file["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
        file["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
    }

    void ParticleEmitterSphere::load(json& file)
    {
        m_enabled =                                                                         file["Enabled"];
        m_emissionRate =                                                                    file["EmissionRate"];
        std::vector<float> position =                                                       file["Position"];
        m_emitterConstantBufferData.position = Vector4(&position[0]);
        std::vector<float> scale =                                                          file["Scale"];
        m_emitterConstantBufferData.scale = Vector4(&scale[0]);
        
        std::vector<float> defaultRotation = { 0.0f, 0.0f, 0.0f };
        m_rotation =                                                                        file.value("Rotation", defaultRotation);
        m_emitterConstantBufferData.rotation = Matrix::CreateRotationX(m_rotation[0]) * Matrix::CreateRotationY(m_rotation[1]) * Matrix::CreateRotationZ(m_rotation[2]);
        m_emitterConstantBufferData.rotation = m_emitterConstantBufferData.rotation.Transpose();

        std::vector<float> partitioning =                                                   file["Partitioning"];
        m_emitterConstantBufferData.partitioning = Vector4(&partitioning[0]);
        m_emitterConstantBufferData.particleOrientation =                                   file["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed =                                    file["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan =                                     file["LifeSpan"];
        m_emitterConstantBufferData.particlesMass =                                         file["Mass"];
        std::vector<float> color =                                                          file["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart =                                     file["Size start"];
        m_emitterConstantBufferData.particleSizeEnd =                                       file["Size end"];

        float rotation[3] = { 0.0f, 0.0f, 0.0f };
        ImGuizmo::RecomposeMatrixFromComponentsRadians((float*)&m_emitterConstantBufferData.position, (float*)&m_rotation[0], (float*)&m_emitterConstantBufferData.scale, m_worldf);
        

        ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&rotation, (float*)&m_emitterConstantBufferData.partitioning, m_fakeWorldf);
    }

    std::string ParticleEmitterSphere::toString()
    {
        return std::string("(Sphere)") + m_name;
    }

}