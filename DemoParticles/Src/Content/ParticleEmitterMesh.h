#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;
    class ModelLoader;
    class ComputeShader;

    class ParticleEmitterMesh : public IParticleEmitter
    {
    public:
        ParticleEmitterMesh(const DX::DeviceResources* deviceResources, std::string name);
        
        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;
        virtual void RenderImGui(Camera* camera) override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;
        virtual std::string toString() override;

    private:
        
        std::unique_ptr<ModelLoader> m_modelLoader; //TODO make a singleton or simple object not pointer
        std::unique_ptr<Model> m_model;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_meshSRV;


        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterMeshConstantBuffer                           m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        float m_lastEmitTime = 0.0f;
        bool m_needEmit = false;

        DirectX::SimpleMath::Vector3 m_scale = DirectX::SimpleMath::Vector3(1.0f, 1.0f, 1.0f);
        DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);
        DirectX::SimpleMath::Vector3 m_rotation = DirectX::SimpleMath::Vector3(0.0f, 0.0f, 0.0f);

        float m_worldf[16] = 
        { 1.f, 0.f, 0.f, 0.f,
          0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f,
          0.f, 0.f, 0.f, 1.f };

        bool m_guizmoHidden = true;
        ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_guizmoMode = ImGuizmo::WORLD;

        int m_currentMeshPart = 0;
    };
}
