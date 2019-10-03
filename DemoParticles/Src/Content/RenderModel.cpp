#include "pch.h"
#include "RenderModel.h"

#include "Model/ModelLoader.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/InputManager.h"
#include "Common/Shader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderModel::RenderModel(const DX::DeviceResources* deviceResources)
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
        m_model = m_modelLoader->load("CatMac.fbx");

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderModel_VS.cso", L"RenderModel_PS.cso", m_model->getInputElements());

        
        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );

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
        
        //Z rotation is temporary as I need to know why the model is upside down
        m_world = Matrix::CreateScale(1.0f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }

    void RenderModel::createWindowSizeDependentResources()
    {
        
    }

    void RenderModel::releaseDeviceDependentResources()
    {
       //TO DO
    }

    void RenderModel::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        assert(camera);

        Matrix worldViewProj = m_world * camera->getViewProjection();
        XMStoreFloat4x4(&m_constantBufferData.worldViewProj, worldViewProj.Transpose());
    }

    void RenderModel::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();
        
        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
        
        for (int i = 0; i < m_model->getMeshCount(); ++i)
        {
            UINT stride = m_model->getVertexStride();//sizeof(VertexObject);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->IASetInputLayout(m_shader->getInputLayout());

            context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

            context->RSSetState(m_rasterizerState.Get());

            context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_model->getMesh(i)->getIndexCount(), 0, 0);
        }
    }

    void RenderModel::updateConstantBuffer()
    {
        Matrix worldViewProj = m_world * m_camera->getViewProjection();
        XMStoreFloat4x4(&m_constantBufferData.worldViewProj, worldViewProj.Transpose());
    }

}