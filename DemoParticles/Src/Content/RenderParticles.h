#pragma once

#include "IRenderable.h"

#include "Content/ShaderStructures.h"

namespace DemoParticles
{
    class ComputeShader;
    class SortLib;

    class RenderParticles : public IRenderable
    {
    public:
        RenderParticles(const DX::DeviceResources* deviceResources);

        virtual void init() override;
        virtual void release() override;

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

        void setShaderResourceViews(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionView, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalView);
        void setBakedParticleUAV(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> backedParticleUAV) { m_bakedParticlesUAV = backedParticleUAV; }
        void setBakedIndirectArgs(Microsoft::WRL::ComPtr<ID3D11Buffer> bakedIndirectArgsBuffer) { m_bakedIndirectArgsBuffer = bakedIndirectArgsBuffer; }

    private:

        void resetParticles();
        void emitParticles();
        void simulateParticles();
        void initEmitters();

        struct Particle
        {
            DirectX::SimpleMath::Vector4 position;
            DirectX::SimpleMath::Vector4 normal;
            DirectX::SimpleMath::Vector4 velocity;
            
            float lifeSpan;
            float age;
            float mass;
        };

        struct ParticleIndexElement
        {
            float distance; //squared distance from camera
            float index; //index in the particle buffer
        };

        enum class ForceFieldTypes
        {
            Point = 0,
            Plane = 1,
        };

        std::unique_ptr<Shader>     m_moveShader;

        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_vertexBuffer;
        UINT                                    m_vertexStride;

        WorldConstantBuffer                 m_worldConstantBufferData;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pointSamplerState;

        DirectX::SimpleMath::Matrix     m_world;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_positionView;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_normalView;

        int m_nbParticles;
        int m_maxParticles = 1000 * 1024;//500 * 1024; //need to augment MAX_NUM_TG in sortLib.cpp

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_particleBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_particleSRV;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_particleUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_deadListUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveIndexBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_aliveIndexSRV;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_aliveIndexUAV;

        std::unique_ptr<ComputeShader>                      m_initDeadListShader;
        std::unique_ptr<ComputeShader>                      m_emitParticles;
        std::unique_ptr<ComputeShader>                      m_emitFromBufferParticles;

        EmitterConstantBuffer                               m_emitterConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterConstantBuffer;

        EmitterFromBufferConstantBuffer                     m_emitterFromBufferConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_emitterFromBufferConstantBuffer;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_deadListCountConstantBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_aliveListCountConstantBuffer;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_indirectDrawArgsBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_indirectDrawArgsUAV;

        std::unique_ptr<ComputeShader>                      m_simulateShader;

        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>   m_bakedParticlesUAV;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_bakedIndirectArgsBuffer;

        ParticlesGlobalsConstantBuffer                      m_particlesGlobalSettingsBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_particlesGlobalSettingsBuffer;

        SimulateParticlesConstantBuffer                     m_simulateParticlesBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_simulateParticlesBuffer;

        ForceField                                          m_forceFieldsList[MAX_FORCE_FIELDS];
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_forceFieldsBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_forceFieldsSRV;

        bool m_resetParticles = true;

        std::unique_ptr<SortLib>                            m_sortLib;

        //temp
        float m_emitFrequence = 0.0f;
    };
}
