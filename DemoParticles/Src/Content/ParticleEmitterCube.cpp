#include "pch.h"
#include "ParticleEmitterCube.h"

#include "Common/ComputeShader.h"
#include "Camera/Camera.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleEmitterCube::ParticleEmitterCube(const DX::DeviceResources* deviceResources, std::string name)
        : IParticleEmitter(deviceResources, name, EmitterType::ET_Cube)
    {
        //m_emitDelay = 0.1f;
        m_emitterConstantBufferData.maxSpawn = 200000;
        m_emitterConstantBufferData.particleOrientation = 0;
        m_emitterConstantBufferData.particlesBaseSpeed = 0.0f;
        m_emitterConstantBufferData.particlesLifeSpan = 3.0f;
        m_emitterConstantBufferData.particlesMass = 1.0f;
        m_emitterConstantBufferData.colorStart = Color(1.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.colorEnd = Color(1.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.particleSizeStart = 0.01f;
        m_emitterConstantBufferData.particleSizeEnd = 0.01f;

        //m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(1.0f / 480.0f, 1.0f / 480.0f, 80.0f/480.0f - 1.0f / 480.0f, 80.0f/480.0f - 1.0f / 480.0f);
        m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 512.0f / 2048.0f, 512.0f / 2048.0f);
        //m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(512.0f / 2048.0f, 0.0f, 512.0f / 2048.0f, 512.0f / 2048.0f);
        //m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 1.0f, 1.0f);
    }

    void ParticleEmitterCube::createDeviceDependentResources()
    {
        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticlesCube_CS.cso");

        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterCubeConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );

        DX::ThrowIfFailed(
            //CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"Particle.dds", &m_particleTexture1, &m_particleTexture1SRV)
            //CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"smoke.dds", &m_particleTexture1, &m_particleTexture1SRV)
            //CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"particlesSheet.dds", &m_particleTexture1, &m_particleTexture1SRV)
            CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"particlesSheetPerso.dds", &m_particleTexture1, &m_particleTexture1SRV)
            //CreateDDSTextureFromFile(m_deviceResources->GetD3DDevice(), L"smokePerso.dds", &m_particleTexture1, &m_particleTexture1SRV)
        );
    }

    void ParticleEmitterCube::update(DX::StepTimer const& timer)
    {
        if (!m_enabled)
        {
            return;
        }

        if (m_emissionRate > 0.0f)
        {
            m_emissionRateAccumulation += m_emissionRate * (float)timer.GetElapsedSeconds();

            if (m_emissionRateAccumulation > 1.0f)
            {
                float integerPart = 0.0f;
                float fraction = modf(m_emissionRateAccumulation, &integerPart);
                m_emitterConstantBufferData.maxSpawn = (UINT)integerPart;
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

        m_emitterConstantBufferData.world = Matrix::CreateScale(m_scale);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationX(m_rotation.x);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationY(m_rotation.y);
        m_emitterConstantBufferData.world *= Matrix::CreateRotationZ(m_rotation.z);
        m_emitterConstantBufferData.world *= Matrix::CreateTranslation(m_position);
        m_emitterConstantBufferData.world = m_emitterConstantBufferData.world.Transpose();
        
    }

    void ParticleEmitterCube::emit()
    {
        if (!m_enabled || m_emitterConstantBufferData.maxSpawn == 0)
        {
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        m_emitParticles->setConstantBuffer(4, m_emitterConstantBuffer);
        m_emitParticles->begin();
        //maxSpawn / 256 as group max so and in shader it's 256 so we spawn maxspawn aligned to 256 threads
        m_emitParticles->start(DX::align(m_emitterConstantBufferData.maxSpawn, 256) / 256, 1, 1);
        m_emitParticles->end();
    }

    void ParticleEmitterCube::RenderImGui(Camera* camera)
    {
        if (ImGui::TreeNode(toString().c_str()))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            ImGui::DragInt("uv X", &m_uvTileX, 1, 0, 2);
            ImGui::DragInt("uv y", &m_uvTileY, 1, 0, 0);

            //m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(uvTileX * 80.0f / 480.0f + 1.0f / 480.0f, uvTileY * 80.0f / 480.0f + 1.0f / 480.0f, 80.0f / 480.0f - 1.0f / 480.0f, 80.0f / 480.0f - 1.0f / 480.0f);
            m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(m_uvTileX * 512.0f / 2048.0f, m_uvTileY * 512.0f / 2048.0f, 512.0f / 2048.0f, 512.0f / 2048.0f);
            //m_emitterConstantBufferData.uvSprite = DirectX::SimpleMath::Vector4(0.0, 0.0, 1.0, 1.0);
            ImGui::Image((ImTextureID)m_particleTexture1SRV.Get(), ImVec2(64.0f, 64.0f), ImVec2(m_emitterConstantBufferData.uvSprite.x, m_emitterConstantBufferData.uvSprite.y), ImVec2(m_emitterConstantBufferData.uvSprite.x + m_emitterConstantBufferData.uvSprite.z, m_emitterConstantBufferData.uvSprite.y + m_emitterConstantBufferData.uvSprite.w));

            //ImGui::DragInt("Max Spawn", (int*)&m_emitterConstantBufferData.maxSpawn, 1, 0, 10000000);
            ImGui::DragFloat("Emission Rate", (float*)&m_emissionRate, 1.0f, 0.0f, 10000000.0f);
            //ImGui::DragFloat3("Position", (float*)&m_emitterConstantBufferData.position, 0.01f);
            const char* orientationItems[] = { "Billboard", "Backed Normal", "Direction" };
            ImGui::Combo("Particles orientation", (int*)&m_emitterConstantBufferData.particleOrientation, orientationItems, 3);
            ImGui::DragFloat("Base speed", &m_emitterConstantBufferData.particlesBaseSpeed, 0.1f, 0.0f, 100.0f);
            ImGui::DragFloat("LifeSpan", &m_emitterConstantBufferData.particlesLifeSpan, 0.1f, -1.0f, 100.0f);
            ImGui::DragFloat("Mass", &m_emitterConstantBufferData.particlesMass, 0.1f, 0.0f, 100.0f);
            ImGui::ColorEdit4("Color Start", (float*)&m_emitterConstantBufferData.colorStart);
            ImGui::ColorEdit4("Color End", (float*)&m_emitterConstantBufferData.colorEnd);
            ImGui::DragFloat("Size Start", &m_emitterConstantBufferData.particleSizeStart, 0.01f, 0.0f, 10.0f);
            ImGui::DragFloat("Size End", &m_emitterConstantBufferData.particleSizeEnd, 0.01f, 0.0f, 10.0f);

            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_position, (float*)&m_rotation, (float*)&m_scale);
            ImGui::DragFloat3("Position", (float*)&m_position, 0.01f);
            ImGui::DragFloat3("Scale", (float*)&m_scale, 0.01f);
            ImGui::DragFloat3("Rotation", (float*)&m_rotation, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_position, (float*)&m_rotation, (float*)&m_scale, m_worldf);

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
            if (ImGui::RadioButton("Translate", m_guizmoOperation == ImGuizmo::TRANSLATE && m_guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::TRANSLATE;
                m_guizmoHidden = false;
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Scale", m_guizmoOperation == ImGuizmo::SCALE && m_guizmoHidden == false))
            {
                m_guizmoOperation = ImGuizmo::SCALE;
                m_guizmoHidden = false;
            }

            ImGui::SameLine();
            if (ImGui::RadioButton("Rotation", m_guizmoOperation == ImGuizmo::ROTATE && m_guizmoHidden == false))
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

                ImGuizmo::SetID(m_guizmoUniqueID);
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
                ImGuizmo::Manipulate(&camera->getView().m[0][0], &camera->getProjection().m[0][0], m_guizmoOperation, m_guizmoMode, m_worldf, nullptr, /*snap*/nullptr);
                ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_position, (float*)&m_rotation, (float*)&m_scale);
            }

            ImGui::TreePop();
        }
    }

    void ParticleEmitterCube::save(json& file)
    {
        file["Type"] = m_type;
        file["Name"] = m_name;
        file["Enabled"] = m_enabled;
        file["uvTileX"] = m_uvTileX;
        file["uvTileY"] = m_uvTileY;
        file["EmissionRate"] = m_emissionRate;
        file["Particles orientation"] = m_emitterConstantBufferData.particleOrientation;
        file["Base speed"] = m_emitterConstantBufferData.particlesBaseSpeed;
        file["LifeSpan"] = m_emitterConstantBufferData.particlesLifeSpan;
        file["Mass"] = m_emitterConstantBufferData.particlesMass;
        file["Color"] = { m_emitterConstantBufferData.colorStart.R(), m_emitterConstantBufferData.colorStart.G(), m_emitterConstantBufferData.colorStart.B(), m_emitterConstantBufferData.colorStart.A() };
        file["Color end"] = { m_emitterConstantBufferData.colorEnd.R(), m_emitterConstantBufferData.colorEnd.G(), m_emitterConstantBufferData.colorEnd.B(), m_emitterConstantBufferData.colorEnd.A() };
        file["Size start"] = m_emitterConstantBufferData.particleSizeStart;
        file["Size end"] = m_emitterConstantBufferData.particleSizeEnd;
        file["Position"] = { m_position.x, m_position.y, m_position.z };
        file["Scale"] = { m_scale.x, m_scale.y, m_scale.z };
        file["Rotation"] = { m_rotation.x, m_rotation.y, m_rotation.z };
        

    }

    void ParticleEmitterCube::load(json& file)
    {
        m_enabled =                                                                 file["Enabled"];
        m_uvTileX =                                                                 file["uvTileX"];
        m_uvTileY =                                                                 file["uvTileY"];
        m_emissionRate =                                                            file["EmissionRate"];
        //std::vector<float> position = file["Emitters"]["Cube"]["Position"];
        //m_emitterConstantBufferData.position = Vector4(&position[0]);
        m_emitterConstantBufferData.particleOrientation =                           file["Particles orientation"];
        m_emitterConstantBufferData.particlesBaseSpeed =                            file["Base speed"];
        m_emitterConstantBufferData.particlesLifeSpan =                             file["LifeSpan"];
        m_emitterConstantBufferData.particlesMass =                                 file["Mass"];
        std::vector<float> color =                                                  file["Color"];
        m_emitterConstantBufferData.colorStart = Vector4(&color[0]);
        std::vector<float> colorEnd =                                               file["Color end"];
        m_emitterConstantBufferData.colorEnd = Vector4(&colorEnd[0]);
        m_emitterConstantBufferData.particleSizeStart = file["Size start"];
        m_emitterConstantBufferData.particleSizeEnd =                               file["Size end"];

        std::vector<float> position =                                               file["Position"];
        m_position= Vector3(&position[0]);

        std::vector<float> scale =                                                  file["Scale"];
        m_scale = Vector3(&scale[0]);

        std::vector<float> rotation =                                               file["Rotation"];
        m_rotation = Vector3(&rotation[0]);

        ImGuizmo::RecomposeMatrixFromComponents((float*)&m_position, (float*)&m_rotation, (float*)&m_scale, m_worldf);
    }

    std::string ParticleEmitterCube::toString()
    {
        return std::string("(Cube)") + m_name;
    }

}