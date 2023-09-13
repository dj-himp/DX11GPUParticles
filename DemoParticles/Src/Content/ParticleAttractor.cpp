#include "pch.h"
#include "ParticleAttractor.h"

#include "../Common/ComputeShader.h"
#include "../Camera/Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleAttractor::ParticleAttractor(const DX::DeviceResources* deviceResources, std::string name)
        : m_deviceResources(deviceResources)
        , m_name(name)
    {
        
    }

    void ParticleAttractor::createDeviceDependentResources()
    {
        
    }

    void ParticleAttractor::update(DX::StepTimer const& timer)
    {

    }

    void ParticleAttractor::apply(float maxParticles, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   indirectDrawArgsUAV, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   particleUAV)
    {
        if (!m_enabled)
        {
            return;
        }
    }

    void ParticleAttractor::RenderImGui(Camera* camera)
    {
        /*if (ImGui::TreeNode(toString().c_str()))
        {
            ImGui::Checkbox("Enabled", &m_enabled);
            ImGui::DragFloat("Gravity", &m_attractorConstantBufferData.gravity, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Mass", &m_attractorConstantBufferData.mass, 0.1f, 0.1f, 100.0f);
            ..ImGui::DragFloat("KillZone radius", &m_attractorConstantBufferData.killZoneRadius, 0.1f, 0.1f, 100.0f);

            float scale[3];
            float rotation[3];
            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_attractorConstantBufferData.position, rotation, scale);
            ImGui::DragFloat3("Position", (float*)&m_attractorConstantBufferData.position, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_attractorConstantBufferData.position, rotation, scale, m_worldf);

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

            if (guizmoHidden == false)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                ImGuizmo::Manipulate(&camera->getView().Transpose().m[0][0], &camera->getProjection().Transpose().m[0][0], m_guizmoOperation, m_guizmoMode, m_worldf, nullptr, nullptr);
                ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_attractorConstantBufferData.position, rotation, scale);
            }

            ImGui::TreePop();
        }*/
    }

    void ParticleAttractor::save(json& file)
    {

    }

    void ParticleAttractor::load(json& file)
    {

    }

    std::string ParticleAttractor::toString()
    {
        return m_name;
    }
}
