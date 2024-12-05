#pragma once
#include "IParticleEmitter.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;

    class ParticleEmitterSphere : public IParticleEmitter
    {
    public:
        ParticleEmitterSphere(const DX::DeviceResources* deviceResources, std::string name);
        
        virtual void createDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer) override;
        virtual void emit() override;
        virtual void RenderImGui(Camera* camera) override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;
        virtual std::string toString() override;

    private:
        
        std::unique_ptr<ComputeShader>                      m_emitParticles;

        EmitterSphereConstantBuffer                         m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        float m_lastEmitTime = 0.0f;
        bool m_needEmit = false;

        float m_worldf[16] =
        { 1.f, 0.f, 0.f, 0.f,
          0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f,
          0.f, 0.f, 0.f, 1.f };

        //Fake world matrix to add a scale gyzmo to the "partitioning" variable
        float m_fakeWorldf[16] =
        { 1.f, 0.f, 0.f, 0.f,
          0.f, 1.f, 0.f, 0.f,
          0.f, 0.f, 1.f, 0.f,
          0.f, 0.f, 0.f, 1.f };

        std::vector<float> m_rotation = { 0.0f, 0.0f, 0.0f };

        bool m_guizmoHidden = true;
        ImGuizmo::OPERATION m_guizmoOperation = ImGuizmo::TRANSLATE;
        ImGuizmo::MODE m_guizmoMode = ImGuizmo::WORLD;

        float m_emissionRate = 10000.0f;
        float m_emissionRateAccumulation = 0.0f;
        
        Microsoft::WRL::ComPtr<ID3D11Resource>              m_particleTexture1;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleTexture1SRV;
        int m_uvTileX = 0;
        int m_uvTileY = 0;

        //TEMP

        choreograph::Timeline m_timeline;
        choreograph::Output<DirectX::SimpleMath::Vector4> m_ChoreoPosition;
    };
}
