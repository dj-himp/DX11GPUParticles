#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

    class ParticleEmitterPoint : public IParticleEmitter
    {
    public:
        ParticleEmitterPoint(const DX::DeviceResources* deviceResources, std::string name);
        
        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;
        virtual void RenderImGui(Camera* camera) override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;
        virtual std::string toString() override;

    private:
        
        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterPointConstantBuffer                         m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        float m_emissionRate = 10000.0f;
        float m_emissionRateAccumulation = 0.0f;


        DirectX::SimpleMath::Vector3 m_emitterRotation;

        float m_worldf[16] = 
        { 1.f, 0.f, 0.f, 0.f,
          0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f,
          0.f, 0.f, 0.f, 1.f };

        bool m_guizmoHidden = true;
        ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_guizmoMode = ImGuizmo::WORLD;


        Microsoft::WRL::ComPtr<ID3D11Resource>              m_particleTexture1;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleTexture1SRV;
        int m_uvTileX = 0;
        int m_uvTileY = 0;
    };
}
