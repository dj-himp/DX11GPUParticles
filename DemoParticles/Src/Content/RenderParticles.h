#pragma once

#include "IRenderable.h"
#include "IParticleEmitter.h"

#include "Content/ShaderStructures.h"
#include "Common/FGAParser.h"

using json = nlohmann::json;

namespace DemoParticles
{
    class VertexShader;
    class GeometryShader;
    class PixelShader;
    class ComputeShader;
    class SortLib;
    class IParticleEmitter;

    class RenderParticles : public IRenderable
    {
    public:
        RenderParticles(const DX::DeviceResources* deviceResources);

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;
        virtual void RenderImGui(Camera* camera) override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;

        void setShaderResourceViews(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionView, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalView);
        void setBakedParticleUAV(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> backedParticleUAV) { m_bakedParticlesUAV = backedParticleUAV; }
        void setBakedIndirectArgs(Microsoft::WRL::ComPtr<ID3D11Buffer> bakedIndirectArgsBuffer) { m_bakedIndirectArgsBuffer = bakedIndirectArgsBuffer; }

    private:

        void resetParticles();
        void emitParticles();
        void simulateParticles();
        void initAttractors();
        void addEmitter(EmitterType type, std::string name);

        void initForceField();
        void updateForceField();
        void renderForceField();

        std::unique_ptr<VertexShader>           m_renderParticleVS;
        std::unique_ptr<GeometryShader>         m_renderParticleGS;
        std::unique_ptr<PixelShader>            m_renderParticlePS;
        std::unique_ptr<GeometryShader>         m_renderParticleLineGS;

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

        Attractor                                           m_attractorList[MAX_ATTRACTORS];
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_attractorsBuffer;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_attractorsSRV;

        bool m_resetParticles = true;

        std::unique_ptr<SortLib>                            m_sortLib;

        Microsoft::WRL::ComPtr<ID3D11Resource>              m_noiseTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_noiseTextureSRV;

        std::vector<std::unique_ptr<IParticleEmitter>>      m_particleEmitters;

        //TEMP
        Microsoft::WRL::ComPtr<ID3D11Texture3D>             m_forceFieldTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_forceFieldTextureSRV;
        FGAParser::FGAContent                               m_content;
        std::unique_ptr<VertexShader>                       m_renderForceFieldVS;
        std::unique_ptr<GeometryShader>                     m_renderForceFieldGS;
        std::unique_ptr<PixelShader>                        m_renderForceFieldPS;
        RenderForceFieldConstantBuffer                      m_renderForceFieldConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_renderForceFieldConstantBuffer;
        bool                                                m_renderForceField = false;
        int                                                 m_forceFieldSampleMode = 0;

        struct ForceField
        {
            std::string m_fileName;
            FGAParser::FGAContent m_content;
            bool m_loaded = false;

            ForceField(std::string fileName) { m_fileName = fileName; }
        };

        std::vector<ForceField>                            m_forceFieldList;
        std::string                                        m_currentForceField = "VF_Vortex.fga";
        size_t                                             m_currentlyLoadedForceField;

        bool m_sortParticles = false;

        std::string m_emitterNameToDelete;

    };
}
