#pragma once

#include "ShaderStructures.h"

using json = nlohmann::json;
    
namespace DemoParticles
{
    class Camera;

    class ParticleAttractor
    {
    public:
        ParticleAttractor(std::string name);

        void update(DX::StepTimer const& timer);
        void RenderImGui(Camera* camera);
        void save(json& file);
        void load(json& file);
        std::string toString();
        std::string getName() { return m_name; }
        Attractor getAttractorParam() { return m_attractorParam; }

    private:

        std::string m_name;
        Attractor m_attractorParam;

        /*bool m_enabled = true;

        DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
        float gravity = 10.0f;
        float mass = 5.0f;
        float killZoneRadius = 0.5f;
        */
        float m_worldf[16] =
        { 1.f, 0.f, 0.f, 0.f,
          0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f,
          0.f, 0.f, 0.f, 1.f };


        bool m_guizmoHidden = true;
        ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_guizmoMode = ImGuizmo::WORLD;
    };
}