#pragma once

#include "ShaderStructures.h"

using json = nlohmann::json;
    
namespace DemoParticles
{
    class Camera;
    class ComputeShader;

    class ParticleAttractor
    {
    public:
        ParticleAttractor(const DX::DeviceResources* deviceResources, std::string name);

        void createDeviceDependentResources();

        void update(DX::StepTimer const& timer);
        void apply(float maxParticles, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   indirectDrawArgsUAV, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   particleUAV);
        void RenderImGui(Camera* camera);
        void save(json& file);
        void load(json& file);
        std::string toString();

    private:
        const DX::DeviceResources* m_deviceResources;

        std::string m_name;
        bool m_enabled = true;

        DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

        float m_worldf[16] =
        { 1.f, 0.f, 0.f, 0.f,
          0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f,
          0.f, 0.f, 0.f, 1.f };

        ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_guizmoMode = ImGuizmo::WORLD;
    };
}