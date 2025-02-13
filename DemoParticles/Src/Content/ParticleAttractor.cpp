#include "pch.h"
#include "ParticleAttractor.h"

#include "../Common/ComputeShader.h"
#include "../Camera/Camera.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    ParticleAttractor::ParticleAttractor(std::string name)
        : m_name(name)
    {
        m_attractorParam.enabled = true;
        m_attractorParam.position = DirectX::SimpleMath::Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        m_attractorParam.gravity = 10.0f;
        m_attractorParam.mass = 5.0f;
        m_attractorParam.killZoneRadius = 0.5f;
    }

    void ParticleAttractor::update(DX::StepTimer const& /*timer*/)
    {

    }

    void ParticleAttractor::RenderImGui(Camera* camera)
    {
        if (ImGui::TreeNode(toString().c_str()))
        {
            ImGui::Checkbox("Enabled", &m_attractorParam.enabled);
            ImGui::DragFloat("Gravity", &m_attractorParam.gravity, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("Mass", &m_attractorParam.mass, 0.1f, 0.1f, 100.0f);
            ImGui::DragFloat("KillZone radius", &m_attractorParam.killZoneRadius, 0.1f, 0.1f, 100.0f);

            float scale[3];
            float rotation[3];
            ImGuizmo::DecomposeMatrixToComponents(m_worldf, (float*)&m_attractorParam.position, rotation, scale);
            ImGui::DragFloat3("Position", (float*)&m_attractorParam.position, 0.01f);
            ImGuizmo::RecomposeMatrixFromComponents((float*)&m_attractorParam.position, rotation, scale, m_worldf);

            if (!m_attractorParam.enabled)
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

            if (m_guizmoHidden == false)
            {
                ImGuiIO& io = ImGui::GetIO();
                ImGuizmo::SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);

                ImGuizmo::Manipulate(&camera->getView().m[0][0], &camera->getProjection().m[0][0], m_guizmoOperation, m_guizmoMode, m_worldf, nullptr, nullptr);
                ImGuizmo::DecomposeMatrixToComponentsRadians(m_worldf, (float*)&m_attractorParam.position, rotation, scale);
            }

            ImGui::TreePop();
        }
    }

    void ParticleAttractor::save(json& file)
    {
		file["Name"] = m_name;
		file["Enabled"] = m_attractorParam.enabled;
        file["Position"] = { m_attractorParam.position.x, m_attractorParam.position.y, m_attractorParam.position.z };
        file["Gravity"] = m_attractorParam.gravity;
        file["Mass"] = m_attractorParam.mass;
        file["KillZoneRadius"] = m_attractorParam.killZoneRadius;
    }

    void ParticleAttractor::load(json& file)
    {
        m_name = file["Name"];
        m_attractorParam.enabled = file["Enabled"];
		std::vector<float> position = file["Position"];
        m_attractorParam.position = Vector4(position[0], position[1], position[2], 1.0f);
        m_attractorParam.gravity = file["Gravity"];
        m_attractorParam.mass = file["Mass"];
        m_attractorParam.killZoneRadius = file["KillZoneRadius"];

		DirectX::SimpleMath::Vector3 scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
		DirectX::SimpleMath::Vector3 rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
        ImGuizmo::RecomposeMatrixFromComponents((float*)&m_attractorParam.position, (float*)&rotation, (float*)&scale, m_worldf);
    }

    std::string ParticleAttractor::toString()
    {
        return m_name;
    }
}
