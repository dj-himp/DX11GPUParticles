#include "pch.h"
#include "ParticleEmitterMesh.h"

#include "Model/ModelLoader.h"
#include "Model/Model.h"
#include "Common/ComputeShader.h"
#include "Camera/Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterMesh::ParticleEmitterMesh(const DX::DeviceResources* deviceResources, std::string name)
        : IParticleEmitter(deviceResources, name)
    {
        m_emitterConstantBufferData.maxSpawn = 100;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 1.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.color = Color(0.5f, 0.2f, 0.2f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.01f;

    }

    void ParticleEmitterMesh::createDeviceDependentResources()
    {
        m_modelLoader = std::make_unique<ModelLoader>(m_deviceResources);
        //m_model = m_modelLoader->load("CatMac.fbx");
        m_model = m_modelLoader->load("TrexByJoel3d.fbx", true);
        //m_model = m_modelLoader->load("deer.fbx");

        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticlesMesh_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterPointConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );
    }

    void ParticleEmitterMesh::update(DX::StepTimer const& timer)
    {
        if (!m_enabled)
        {
            return;
        }

        //false to reset if the previous render emit particles
        m_needEmit = false;

        m_lastEmitTime -= (float)timer.GetElapsedSeconds();
        //m_lastEmitTime -= (float)timer.GetElapsedSeconds() / 1000.0f;
        if (m_lastEmitTime <= 0.0)
        {
            m_lastEmitTime = m_emitDelay;
            m_needEmit = true;
        }

        m_emitterConstantBufferData.world = Matrix::CreateScale(m_scale);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationX(m_rotation.x);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationY(m_rotation.y);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationZ(m_rotation.z);
        m_emitterConstantBufferData.world *= Matrix::CreateTranslation(m_position);
        m_emitterConstantBufferData.world = m_emitterConstantBufferData.world.Transpose();
        
    }

    void ParticleEmitterMesh::emit()
    {
        if (!m_enabled || !m_needEmit)
        {
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitParticles->setSRV(0, m_model->getMesh(m_currentMeshPart)->getVertexSRV());
        m_emitParticles->setSRV(1, m_model->getMesh(m_currentMeshPart)->getIndexSRV());
        m_emitParticles->begin();
        m_emitParticles->start(DX::align(m_emitterConstantBufferData.maxSpawn, 1024) / 1024, 1, 1);
        m_emitParticles->end();
    }

    void ParticleEmitterMesh::RenderImGui(Camera* camera)
    {
        if (ImGui::TreeNode(toString().c_str()))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            ImGui::DragInt("mesh Part", &m_currentMeshPart, 1.0f, 0, m_model->getMeshCount() - 1);
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

            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_position, (float*)&m_rotation, (float*)&m_scale);
            ImGui::DragFloat3("Position", (float*)&m_position, 0.01f);
            ImGui::DragFloat3("Scale", (float*)&m_scale, 0.01f);
            ImGui::DragFloat3("Rotation", (float*)&m_rotation, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_position, (float*)&m_rotation, (float*)&m_scale, m_worldf);

            static bool guizmoHidden = true;
            if (!m_enabled)
            {
                guizmoHidden = true;
            }

            if (ImGui::RadioButton("None", guizmoHidden))
            {
                guizmoHidden = true;
            }
            ImGui::SameLine();

            static float snap[3] = { 0.1f, 0.1f, 0.1f };
            if (ImGui::RadioButton("Translate", m_guizmoOperation == ImGuizmo::TRANSLATE && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::TRANSLATE;
                guizmoHidden = false;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_guizmoOperation == ImGuizmo::SCALE && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::SCALE;
                guizmoHidden = false;
            }

            ImGui::SameLine();
            if (ImGui::RadioButton("Rotation", m_guizmoOperation == ImGuizmo::ROTATE && guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::ROTATE;
                guizmoHidden = false;
            }

            if (guizmoHidden == false)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
                ImGuizmo::Manipulate(&camera->getView().Transpose().m[0][0], &camera->getProjection().Transpose().m[0][0], m_guizmoOperation, m_guizmoMode, m_worldf, nullptr, /*snap*/nullptr);
                ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_position, (float*)&m_rotation, (float*)&m_scale);
            }

            ImGui::TreePop();
        }
        
    }

    void ParticleEmitterMesh::save(json& file)
    {
        file["Emitters"]["Mesh"]["Enabled"] = m_enabled;
        file["Emitters"]["Mesh"]["Max Spawn"] = m_emitterConstantBufferData.maxSpawn;
        //file["Emitters"]["Mesh"]["Position"] = { m_emitterConstantBufferData.position.x, m_emitterConstantBufferData.position.y, m_emitterConstantBufferData.position.z, m_emitterConstantBufferData.position.w };
        file["Emitters"]["Mesh"]["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Emitters"]["Mesh"]["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["Emitters"]["Mesh"]["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Emitters"]["Mesh"]["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Emitters"]["Mesh"]["Color"] = { m_emitterConstantBufferData.color.R(), m_emitterConstantBufferData.color.G(), m_emitterConstantBufferData.color.B(), m_emitterConstantBufferData.color.A() };
        file["Emitters"]["Mesh"]["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Emitters"]["Mesh"]["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
        file["Emitters"]["Mesh"]["Position"] = { m_position.x, m_position.y, m_position.z };
        file["Emitters"]["Mesh"]["Scale"] = { m_scale.x, m_scale.y, m_scale.z };
        file["Emitters"]["Mesh"]["Rotation"] = { m_rotation.x, m_rotation.y, m_rotation.z };
    }

    void ParticleEmitterMesh::load(json& file)
    {
        m_enabled = file["Emitters"]["Mesh"]["Enabled"];
        m_emitterConstantBufferData.maxSpawn = file["Emitters"]["Mesh"]["Max Spawn"];
        //std::vector<float> position = file["Emitters"]["Mesh"]["Position"];
        //m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation = file["Emitters"]["Mesh"]["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed = file["Emitters"]["Mesh"]["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan = file["Emitters"]["Mesh"]["LifeSpan"];
        m_emitterConstantBufferData.particlesMass = file["Emitters"]["Mesh"]["Mass"];
        std::vector<float> color = file["Emitters"]["Mesh"]["Color"];
        m_emitterConstantBufferData.color = Vector4(&color[0]);
        m_emitterConstantBufferData.particleSizeStart = file["Emitters"]["Mesh"]["Size start"];
        m_emitterConstantBufferData.particleSizeEnd = file["Emitters"]["Mesh"]["Size end"];

        std::vector<float> position = file["Emitters"]["Mesh"]["Position"];
        m_position = Vector3(&position[0]);

        std::vector<float> scale = file["Emitters"]["Mesh"]["Scale"];
        m_scale = Vector3(&scale[0]);

        std::vector<float> rotation = file["Emitters"]["Mesh"]["Rotation"];
        m_rotation = Vector3(&rotation[0]);

        ImGuizmo::RecomposeMatrixFromComponents((float*)&m_position, (float*)&m_rotation, (float*)&m_scale, m_worldf);
    }

    std::string ParticleEmitterMesh::toString()
    {
        return std::string("(Mesh)") + m_name;
    }

}