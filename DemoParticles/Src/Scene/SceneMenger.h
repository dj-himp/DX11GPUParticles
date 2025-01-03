#pragma once
#include "IScene.h"

#include "Content/ShaderStructures.h"

namespace DemoParticles
{
    class MengerRenderer;
    class BakeModelParticles;
    class RenderTarget;
    class RenderFullscreenQuad;
    class ComputeShader;
    class RenderParticles;
    class RenderForceField;
    class RenderModelAndEmitBuffer;
    class RenderModel;

    class SceneMenger : public IScene
    {
    public:
        SceneMenger(const DX::DeviceResources* deviceResources);
        ~SceneMenger();

        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;
        virtual void RenderImGui(Camera* camera) override;
        virtual void save(json& file) override;
        virtual void load(json& file) override;


        //TEMP
        int getCurrentParticlesCount();
    private:

        //std::unique_ptr<MengerRenderer> m_mengerRenderer;
        std::unique_ptr<BakeModelParticles> m_bakeModelParticles;
        std::unique_ptr<RenderFullscreenQuad> m_fullScreenQuad;
        std::unique_ptr<RenderParticles> m_renderParticles;
        std::unique_ptr<RenderModel> m_renderModel;


        std::unique_ptr<RenderTarget> m_rtBakePositions;
        std::unique_ptr<RenderTarget> m_rtBakeNormals;

        std::unique_ptr<ComputeShader> m_computePackParticle;

        bool                            m_bakingDone = false;

        SceneConstantBuffer                     m_sceneConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_sceneConstantBuffer;


        //baking
        struct BackedParticle
        {
            DirectX::SimpleMath::Vector4 position;
            DirectX::SimpleMath::Vector4 normal;
        };

        std::unique_ptr<ComputeShader> m_packShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_backedBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_bakedUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indirectComputeArgsBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_indirectComputeArgsUAV;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indirectComputeConstantBuffer;

        std::unique_ptr<ComputeShader> m_initIndirectComputeArgsShader;

        D3D11_VIEWPORT m_bakingViewport;

        int m_maxBakeBufferSize;
    };
}
