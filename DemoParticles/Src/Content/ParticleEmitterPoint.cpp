#include "pch.h"
#include "ParticleEmitterPoint.h"

#include "Common/ComputeShader.h"
#include "Camera/Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterPoint::ParticleEmitterPoint(const DX::DeviceResources* deviceResources, std::string name)
        : IParticleEmitter(deviceResources, name)
    {
        m_emitterConstantBufferData.rotation = Matrix::CreateFromYawPitchRoll(0.0f, 0.0f, 0.0f);
        m_emitterConstantBufferData.position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.maxSpawn = 100;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 1.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.01f;
        m_emitterConstantBufferData.coneColatitude = 0.0f;
        m_emitterConstantBufferData.coneLongitude = 0.0f;

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

        m_lastEmitTime -= (float)timer.GetElapsedSeconds();
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

    void ParticleEmitterPoint::RenderImGui(Camera* camera)
    {
        if (ImGui::TreeNode(toString().c_str()))
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
            ImGui::SliderAngle("Cone Colatitude", &m_emitterConstantBufferData.coneColatitude, 0.0f, 360.0f);
            ImGui::SliderAngle("Cone Longitude", &m_emitterConstantBufferData.coneLongitude, 0.0f, 360.0f);

            float scale[3];
            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_emitterConstantBufferData.position, (float*)&m_emitterRotation, scale);
            ImGui::DragFloat3("Position", (float*)&m_emitterConstantBufferData.position, 0.01f);
            ImGui::DragFloat3("Rotation", (float*)&m_emitterRotation);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_emitterConstantBufferData.position, (float*)&m_emitterRotation, scale, m_worldf);

            if (!m_enabled)
            {
                m_guizmoHidden = true;
            }

            if (ImGui::RadioButton("None", m_guizmoHidden))
            {
                m_guizmoHidden = true;
            }
            ImGui::SameLine();

            static float snap[3] = { 0.1f, 0.1f, 0.1f };
            static float angleSnap[3] = { 1.0f, 1.0f, 1.0f };
            if (ImGui::RadioButton("Translate", m_guizmoOperation == ImGuizmo::TRANSLATE && m_guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::TRANSLATE;
                m_guizmoHidden = false;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Rotate", m_guizmoOperation == ImGuizmo::ROTATE && m_guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::ROTATE;
                m_guizmoHidden = false;
            }

            if (m_guizmoHidden == false)
            {
                if (ImGui::RadioButton("Local", m_guizmoMode == ImGuizmo::LOCAL))
                {
                    m_guizmoMode = ImGuizmo::LOCAL;
                }
                ImGui::SameLine();
                if (ImGui::RadioButton("World", m_guizmoMode == ImGuizmo::WORLD))
                {
                    m_guizmoMode = ImGuizmo::WORLD;
                }

                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                //memcpy(m_viewf, &camera->getView().Transpose().m[0][0], sizeof(m_viewf));
                //memcpy(m_projectionf, &camera->getProjection().Transpose().m[0][0], sizeof(m_projectionf));

                //ImGuizmo::Manipulate(m_viewf, m_projectionf, guizmoOperation, guizmoMode, m_worldf, nullptr, snap);
                ImGuizmo::Manipulate(&camera->getView().Transpose().m[0][0], &camera->getProjection().Transpose().m[0][0], m_guizmoOperation, m_guizmoMode, m_worldf, nullptr, m_guizmoOperation == ImGuizmo::TRANSLATE ? /*snap*/nullptr : angleSnap);

                ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_emitterConstantBufferData.position, (float*)&m_emitterRotation, scale);

                m_emitterConstantBufferData.rotation = Matrix::CreateRotationX(m_emitterRotation.x) * Matrix::CreateRotationY(m_emitterRotation.y) * Matrix::CreateRotationZ(m_emitterRotation.z);
            }

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
        file["Emitters"]["Point"]["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Emitters"]["Point"]["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
        file["Emitters"]["Point"]["Cone Colatitude"] = m_emitterConstantBufferData.coneColatitude;
        file["Emitters"]["Point"]["Cone Longitude"] = m_emitterConstantBufferData.coneLongitude;
        file["Emitters"]["Point"]["Rotation"] = { m_emitterRotation.x, m_emitterRotation.y, m_emitterRotation.z };
    }

    void ParticleEmitterPoint::load(json& file)
    {
        m_enabled = file["Emitters"]["Point"].value("Enabled", false);
        //m_enabled = file["Emitters"]["Point"]["Enabled"];
        m_emitterConstantBufferData.maxSpawn = file["Emitters"]["Point"]["Max Spawn"];
        std::vector<float> position = file["Emitters"]["Point"]["Position"];
        m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation = file["Emitters"]["Point"]["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed = file["Emitters"]["Point"]["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan = file["Emitters"]["Point"]["LifeSpan"];
        m_emitterConstantBufferData.particlesMass = file["Emitters"]["Point"]["Mass"];
        std::vector<float> color = file["Emitters"]["Point"]["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart = file["Emitters"]["Point"]["Size start"];
        m_emitterConstantBufferData.particleSizeEnd = file["Emitters"]["Point"]["Size end"];
        m_emitterConstantBufferData.coneColatitude = file["Emitters"]["Point"]["Cone Colatitude"];
        m_emitterConstantBufferData.coneLongitude = file["Emitters"]["Point"]["Cone Longitude"];
        std::vector<float> rotation = file["Emitters"]["Point"]["Rotation"];
        m_emitterRotation = Vector3(&rotation[0]);

        m_emitterConstantBufferData.rotation = Matrix::CreateRotationX(m_emitterRotation.x) * Matrix::CreateRotationY(m_emitterRotation.y) * Matrix::CreateRotationZ(m_emitterRotation.z);

        float scale[3] = { 1.0f, 1.0f, 1.0f };
        ImGuizmo::RecomposeMatrixFromComponentsRadians((float*)&m_emitterConstantBufferData.position, (float*)&m_emitterRotation, scale, m_worldf);
    }

    std::string ParticleEmitterPoint::toString()
    {
        return std::string("(Point)") + m_name;
    }

}