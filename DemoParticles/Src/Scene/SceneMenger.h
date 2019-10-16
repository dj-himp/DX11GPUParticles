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

    private:

        std::unique_ptr<MengerRenderer> m_mengerRenderer;
        std::unique_ptr<BakeModelParticles> m_bakeModelParticles;
        std::unique_ptr<RenderFullscreenQuad> m_fullScreenQuad;
        std::unique_ptr<RenderParticles> m_renderParticles;

        std::unique_ptr<RenderTarget> m_rtBakePositions;
        std::unique_ptr<RenderTarget> m_rtBakeNormals;

        std::unique_ptr<ComputeShader> m_computePackParticle;

        bool                            m_bakingDone = false;

        SceneConstantBuffer                     m_sceneConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>    m_sceneConstantBuffer;

    };
}
