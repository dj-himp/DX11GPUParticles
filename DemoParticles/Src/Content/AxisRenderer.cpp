#include "pch.h"
#include "AxisRenderer.h"

#include "Model/AxisCreator.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/Shader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    AxisRenderer::AxisRenderer(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }


    /*AxisRenderer::~AxisRenderer()
    {
    }*/

    void AxisRenderer::init()
    {
        IRenderable::init();

        m_world = Matrix::CreateScale(8.0f);
    }

    void AxisRenderer::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void AxisRenderer::createDeviceDependentResources()
    {
        
        AxisCreator axisCreator(m_deviceResources);
        m_axis = axisCreator.create();

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderDebugColor_VS.cso", L"RenderDebugColor_PS.cso", m_axis->getInputElements());

        

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(ModelConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );

        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_NONE;
        rasterizerDesc.FrontCounterClockwise = TRUE; //TRUE because my engine is code with right handed coordinates
        rasterizerDesc.DepthBias = D3D11_DEFAULT_DEPTH_BIAS;
        rasterizerDesc.DepthBiasClamp = D3D11_DEFAULT_DEPTH_BIAS_CLAMP;
        rasterizerDesc.SlopeScaledDepthBias = D3D11_DEFAULT_SLOPE_SCALED_DEPTH_BIAS;
        rasterizerDesc.DepthClipEnable = FALSE;
        rasterizerDesc.ScissorEnable = FALSE;
        rasterizerDesc.MultisampleEnable = FALSE;
        rasterizerDesc.AntialiasedLineEnable = FALSE;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState)
        );

        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        //blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    void AxisRenderer::createWindowSizeDependentResources()
    {
        float yaw = 0.1f;
        float pitch = 0.1f;
        m_camera = std::make_unique<Camera>(Vector3(0.0f, 0.0f, -10.0f), yaw, pitch, 0.0f, m_deviceResources->GetOutputWidth() / m_deviceResources->GetOutputHeight(), DirectX::XM_PI / 4.0f);

        //m_camera = std::make_unique<Camera>(Vector3(0.0f, 0.0f, -10.0f), Vector3(0.0f, 0.0f, 1.0f), m_deviceResources->GetOutputSize().Width / m_deviceResources->GetOutputHeight(), DirectX::XM_PI / 4.0f);

        
        
    }

    void AxisRenderer::releaseDeviceDependentResources()
    {
        
    }

    void AxisRenderer::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        assert(camera);
        
        Matrix worldViewProj = m_world * camera->getViewProjection();
        XMStoreFloat4x4(&m_constantBufferData.worldViewProj, worldViewProj.Transpose());
    }

    void AxisRenderer::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

        for (int i = 0; i < m_axis->getMeshCount(); ++i)
        {
            UINT stride = sizeof(VertexColor);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_axis->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_axis->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(m_axis->getMesh(i)->getPrimitiveTopology());
            context->IASetInputLayout(m_shader->getInputLayout());

            context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

            context->RSSetState(m_rasterizerState.Get());

            context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_axis->getMesh(i)->getIndexCount(), 0, 0);
        }
    }
}
