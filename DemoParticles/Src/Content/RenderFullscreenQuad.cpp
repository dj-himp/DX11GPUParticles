#include "pch.h"
#include "RenderFullscreenQuad.h"

#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/VertexShader.h"
#include "Common/PixelShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderFullscreenQuad::RenderFullscreenQuad(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        
    }

    void RenderFullscreenQuad::createDeviceDependentResources()
    {
        m_quad = MeshFactory::getInstance().createQuad();

        m_quadVS = std::make_unique<VertexShader>(m_deviceResources);
        m_quadVS->load(L"RenderQuad_VS.cso", m_quad->getInputElements());

        m_quadPS = std::make_unique<PixelShader>(m_deviceResources);
        m_quadPS->load(L"RenderQuad_PS.cso");


        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(QuadConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );
    }

    void RenderFullscreenQuad::createWindowSizeDependentResources()
    {
        //NOTHING THERE
    }

    void RenderFullscreenQuad::releaseDeviceDependentResources()
    {
        m_constantBuffer.Reset();
    }

    void RenderFullscreenQuad::update(DX::StepTimer const& /*timer*/, Camera* /*camera*/ /*= nullptr*/)
    {
        //TEST
        //m_posScale = Matrix::CreateTranslation(Vector3(0.0f, 0.0f, 0.0f)) * Matrix::CreateScale(1.0f);
        XMStoreFloat4x4(&m_constantBufferData.posScale, m_posScale.Transpose());

    }

    void RenderFullscreenQuad::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

        UINT stride = (UINT)m_quad->getVertexStride();
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_quad->getMesh(0)->getVertexBuffer().GetAddressOf(), &stride, &offset);
        context->IASetIndexBuffer(m_quad->getMesh(0)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        context->IASetInputLayout(m_quadVS->getInputLayout());

        context->VSSetShader(m_quadVS->getVertexShader(), nullptr, 0);
        context->VSSetConstantBuffers1(1, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

        context->RSSetState(RenderStatesHelper::CullCounterClockwise().Get());
        
        context->PSSetSamplers(0, 1, RenderStatesHelper::PointClamp().GetAddressOf());
        context->PSSetShaderResources(0, 1, m_texture.GetAddressOf());
        context->PSSetShader(m_quadPS->getPixelShader(), nullptr, 0);

        context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get() , 0);
        
        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);

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