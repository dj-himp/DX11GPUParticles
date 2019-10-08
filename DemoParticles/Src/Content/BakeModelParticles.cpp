#include "pch.h"
#include "BakeModelParticles.h"

#include "Model/ModelLoader.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/InputManager.h"
#include "Common/Shader.h"
#include "Common/RenderTarget.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    BakeModelParticles::BakeModelParticles(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }

    void BakeModelParticles::init()
    {
        IRenderable::init();

        
    }

    void BakeModelParticles::release()
    {

    }

    void BakeModelParticles::createDeviceDependentResources()
    {
        m_modelLoader = std::make_unique<ModelLoader>(m_deviceResources);
        m_model = m_modelLoader->load("CatMac.fbx");

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"BakeObjectParticles_VS.cso", L"BakeObjectParticles_PS.cso", m_model->getInputElements());

        
        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(WorldConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBufferVS
            )
        );

        /*D3D11_RASTERIZER_DESC rasterizerDesc;
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
        );*/

        //Z rotation is temporary as I need to know why the model is upside down
        m_world = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }

    void BakeModelParticles::createWindowSizeDependentResources()
    {
        
    }

    void BakeModelParticles::releaseDeviceDependentResources()
    {
       //TO DO
    }

    void BakeModelParticles::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        XMStoreFloat4x4(&m_constantBufferData.world, m_world.Transpose());
    }

    void BakeModelParticles::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();
        
        context->UpdateSubresource1(m_constantBufferVS.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
        
        for (int i = 0; i < m_model->getMeshCount(); ++i)
        {
            UINT stride = m_model->getVertexStride();
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->IASetInputLayout(m_shader->getInputLayout());

            context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers(0, 1, m_constantBufferVS.GetAddressOf());

            context->RSSetState(m_rasterizerState.Get());

            context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_model->getMesh(i)->getIndexCount(), 0, 0);

            
        }
    }
}