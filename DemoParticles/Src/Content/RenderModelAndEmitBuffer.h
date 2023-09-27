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
    class GeometryShader;
    class ComputeShader;

    class RenderModelAndEmitBuffer : public IRenderable
    {
    public:
        RenderModelAndEmitBuffer(const DX::DeviceResources* deviceResources);

        void createDeviceDependentResources() override;
        void createWindowSizeDependentResources() override;
        void releaseDeviceDependentResources() override;

        void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        void render() override;
        void RenderImGui(Camera* camera);

        ID3D11UnorderedAccessView* getParticleUAV() { return m_particleUAV.Get(); }
        ID3D11Buffer* getIndirectArgsBuffer() { return m_indirectDispatchArgsBuffer.Get(); }


    private:

        std::unique_ptr<ModelLoader> m_modelLoader; //TODO make a singleton or simple object not pointer
        std::unique_ptr<Model> m_model;
        std::unique_ptr<Camera> m_camera;

        DirectX::SimpleMath::Matrix m_world;

        DirectX::SimpleMath::Vector3 m_position = DirectX::SimpleMath::Vector3(0.0f);

        WorldConstantBuffer m_constantBufferData;
        std::unique_ptr<VertexShader> m_modelVS;
        std::unique_ptr<PixelShader> m_modelPS;
        std::unique_ptr<GeometryShader> m_modelGS;
        std::unique_ptr<ComputeShader> m_modelResetArgsCS;
        std::unique_ptr<ComputeShader> m_modelInitArgsCS;
        std::unique_ptr<ComputeShader> m_modelEmitCS;


        Microsoft::WRL::ComPtr<ID3D11Buffer> m_particleBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_particleUAV;

        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indirectDispatchArgsBuffer;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> m_indirectDispatchArgsUAV;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_indirectDispatchArgsConstantBuffer;
        InitIndirectComputeArgs1DConstantBuffer m_indirectDispatchArgsConstantBufferData;

        ModelToEmitConstantBuffer m_modelToEmitConstantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer> m_modelToEmitConstantBuffer;

        DirectX::SimpleMath::Vector2 m_scaleDensity;
        DirectX::SimpleMath::Vector2 m_offsetDensity;

    };
}
