#pragma once

#include "IRenderable.h"

#include "ShaderStructures.h"

namespace DemoParticles
{
    class Model;
    class ModelLoader;
    class Camera;
    class VertexShader;
    class PixelShader;

    class RenderModel : public IRenderable
    {
    public:
        RenderModel(const DX::DeviceResources* deviceResources);

        void createDeviceDependentResources() override;
        void createWindowSizeDependentResources() override;
        void releaseDeviceDependentResources() override;

        void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        void render() override;

    private:

        std::unique_ptr<ModelLoader> m_modelLoader; //TODO make a singleton or simple object not pointer
        std::unique_ptr<Model> m_model;
        std::unique_ptr<Camera> m_camera;

        DirectX::SimpleMath::Matrix m_world;

        DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f);

        WorldConstantBuffer m_constantBufferData;

        SkinnedConstantBuffer m_skinnedConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_skinnedConstantBuffer;

        std::unique_ptr<VertexShader> m_modelVS;
        std::unique_ptr<PixelShader> m_modelPS;
    };
}
