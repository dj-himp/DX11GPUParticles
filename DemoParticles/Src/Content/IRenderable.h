#pragma once

#include "..\Common\StepTimer.h"

namespace DemoParticles
{
    class Camera;

    class IRenderable
    {
    public:

        IRenderable(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        virtual ~IRenderable();

        virtual void init();
        virtual void release();

        virtual void createDeviceDependentResources() = 0;
        virtual void createWindowSizeDependentResources() = 0;
        virtual void releaseDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) = 0;
        virtual void render() = 0;

    protected:

        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        //TODO : create a model interface and make assimpmodel and fullscreenQuad inheritants

        //std::unique_ptr<ModelLoader> m_modelLoader;
        //std::unique_ptr<AssimpModel> m_model;

        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBuffer;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState>       m_rasterizerState;

        bool m_loadingComplete;
    };
}
