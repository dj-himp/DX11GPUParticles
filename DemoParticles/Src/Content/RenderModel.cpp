#include "pch.h"
#include "RenderModel.h"

#include <ppltasks.h>
#include "../Common/DirectXHelper.h"

#include "../Model/ModelLoader.h"
#include "../Model/Model.h"
#include "../Camera/Camera.h"
#include "../Common/InputManager.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderModel::RenderModel(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }

    void RenderModel::init()
    {
        IRenderable::init();

        
    }

    void RenderModel::release()
    {

    }

    void RenderModel::createDeviceDependentResources()
    {
        m_modelLoader = std::make_unique<ModelLoader>(m_deviceResources);

        auto loadVSTask = DX::ReadDataAsync(L"RenderModel_VS.cso");
        auto loadPSTask = DX::ReadDataAsync(L"RenderModel_PS.cso");

        // After the vertex shader file is loaded, create the shader and input layout.
        auto createVSTask = loadVSTask.then([this](const std::vector<byte>& fileData) {

            m_model = m_modelLoader->load("CatMac.fbx");
            //m_model = m_modelLoader->load("cube.dae");

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateVertexShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    &m_vertexShader
                )
            );

            std::vector<D3D11_INPUT_ELEMENT_DESC> vertexDesc = m_model->getInputElements();

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateInputLayout(
                    vertexDesc.data(),
                    vertexDesc.size(),
                    &fileData[0],
                    fileData.size(),
                    &m_inputLayout
                )
            );
        });



        // After the pixel shader file is loaded, create the shader and constant buffer.
        auto createPSTask = loadPSTask.then([this](const std::vector<byte>& fileData) {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreatePixelShader(
                    &fileData[0],
                    fileData.size(),
                    nullptr,
                    &m_pixelShader
                )
            );

            CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateBuffer(
                    &constantBufferDesc,
                    nullptr,
                    &m_constantBuffer
                )
            );
        });

        (createPSTask && createVSTask).then([this]() {

            D3D11_RASTERIZER_DESC rasterizerDesc;
            rasterizerDesc.FillMode = D3D11_FILL_SOLID;
            rasterizerDesc.CullMode = D3D11_CULL_BACK;
            rasterizerDesc.FrontCounterClockwise = TRUE; //TRUE because my engine is code with right handed coordinates
            rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
            rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
            rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
            rasterizerDesc.DepthClipEnable = TRUE;
            rasterizerDesc.ScissorEnable = FALSE;
            rasterizerDesc.MultisampleEnable = FALSE;
            rasterizerDesc.AntialiasedLineEnable = FALSE;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState)
            );

            m_loadingComplete = true;
        });
        
        //Z rotation is temporary as I need to know why the model is upside down
        m_world = Matrix::CreateScale(1.0f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }

    void RenderModel::createWindowSizeDependentResources()
    {
        m_camera = std::make_unique<Camera>(Vector3(10.0f, 20.0f, -150.0f), Vector3(0.0f, 0.0f, 1.0f), m_deviceResources->GetOutputSize().Width / m_deviceResources->GetOutputSize().Height, DirectX::XM_PI / 4.0f);

        Matrix worldViewProj = m_world * m_camera->getViewProjection();
        XMStoreFloat4x4(&m_constantBufferData.worldViewProj, worldViewProj.Transpose());
    }

    void RenderModel::releaseDeviceDependentResources()
    {
       //TO DO
    }

    void RenderModel::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        
        
    }

    void RenderModel::render()
    {
        if (!m_loadingComplete)
        {
            return;
        }

        auto context = m_deviceResources->GetD3DDeviceContext();
        
        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
        
        for (int i = 0; i < m_model->getMeshCount(); ++i)
        {
            UINT stride = sizeof(VertexObject);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->IASetInputLayout(m_inputLayout.Get());

            context->VSSetShader(m_vertexShader.Get(), nullptr, 0);
            context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

            context->RSSetState(m_rasterizerState.Get());

            context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

            context->DrawIndexed(m_model->getMesh(i)->getIndexCount(), 0, 0);
        }
    }

    void RenderModel::updateConstantBuffer()
    {
        Matrix worldViewProj = m_world * m_camera->getViewProjection();
        XMStoreFloat4x4(&m_constantBufferData.worldViewProj, worldViewProj.Transpose());
    }

}