#include "pch.h"
#include "RenderFullscreenQuad.h"

#include "Common/DirectXHelper.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/Shader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderFullscreenQuad::RenderFullscreenQuad(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }

    void RenderFullscreenQuad::init()
    {
        IRenderable::init();
    }

    void RenderFullscreenQuad::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void RenderFullscreenQuad::createDeviceDependentResources()
    {
        m_quad = MeshFactory::getInstance().createQuad();

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderQuad_VS.cso", L"RenderQuad_PS.cso", m_quad->getInputElements());

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(QuadConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBufferVS
            )
        );

        CD3D11_SAMPLER_DESC samplerDesc;
        samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
        samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
        samplerDesc.MipLODBias = 0.0f;
        samplerDesc.MaxAnisotropy = 1;
        samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
        samplerDesc.MinLOD = -FLT_MAX;
        samplerDesc.MaxLOD = FLT_MAX;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateSamplerState(&samplerDesc, &m_pointSamplerState)
        );

        D3D11_RASTERIZER_DESC rasterizerDesc;
        ZeroMemory(&rasterizerDesc, sizeof(rasterizerDesc));
        rasterizerDesc.FillMode = D3D11_FILL_SOLID;
        rasterizerDesc.CullMode = D3D11_CULL_BACK;
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

        D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
        ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));
        depthStencilDesc.DepthEnable = false;
        depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        //depthStencilDesc.DepthFunc = D3D11_COMPARISON_NEVER;
        depthStencilDesc.StencilEnable = false;
        //depthStencilDesc.FrontFace.StencilFailOp = depthStencilDesc.FrontFace.StencilDepthFailOp = depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
        //depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
        //depthStencilDesc.BackFace = depthStencilDesc.FrontFace;
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState)
        );

        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(blendDesc));
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        blendDesc.RenderTarget[1].BlendEnable = FALSE;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBlendState(&blendDesc, &m_blendState)
        );
    }

    void RenderFullscreenQuad::createWindowSizeDependentResources()
    {
        //NOTHING THERE
    }

    void RenderFullscreenQuad::releaseDeviceDependentResources()
    {
        m_constantBufferVS.Reset();
    }

    void RenderFullscreenQuad::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        //TEST
        //m_posScale = Matrix::CreateTranslation(Vector3(0.0f, 0.0f, 0.0f)) * Matrix::CreateScale(1.0f);
        XMStoreFloat4x4(&m_constantBufferData.posScale, m_posScale.Transpose());

    }

    void RenderFullscreenQuad::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource1(m_constantBufferVS.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

        UINT stride = m_quad->getVertexStride();
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_quad->getMesh(0)->getVertexBuffer().GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(m_quad->getMesh(0)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(m_shader->getInputLayout());

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
        context->VSSetConstantBuffers1(0, 1, m_constantBufferVS.GetAddressOf(), nullptr, nullptr);

        context->RSSetState(m_rasterizerState.Get());
        
        context->PSSetSamplers(0, 1, m_pointSamplerState.GetAddressOf());
        context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

        context->OMSetDepthStencilState(m_depthStencilState.Get() , 0);
        
        //const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        //context->OMSetBlendState(m_blendState.Get(), blendFactor, 0xffffffff);

        context->DrawIndexed(m_quad->getMesh(0)->getIndexCount(), 0, 0);

        ID3D11ShaderResourceView* shaderResource[1] = { nullptr };
        context->PSSetShaderResources(0, 1, shaderResource);
    }

    void RenderFullscreenQuad::setPosScale(const DirectX::SimpleMath::Matrix posScale)
    {
        m_posScale = posScale;
    }

    void RenderFullscreenQuad::setTexture(const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture)
    {
        m_texture = texture;
    }
}