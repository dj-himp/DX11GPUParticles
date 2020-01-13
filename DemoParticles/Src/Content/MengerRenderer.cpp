#include "pch.h"
#include "MengerRenderer.h"

#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/VertexShader.h"
#include "Common/PixelShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    MengerRenderer::MengerRenderer(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        
    }

    void DemoParticles::MengerRenderer::createDeviceDependentResources()
    {
        m_quad = MeshFactory::getInstance().createQuad();

        m_mengerShaderVS = std::make_unique<VertexShader>(m_deviceResources);
        m_mengerShaderVS->load(L"RenderMenger_VS.cso", m_quad->getInputElements());

        m_mengerShaderPS = std::make_unique<PixelShader>(m_deviceResources);
        m_mengerShaderPS->load(L"RenderMenger_PS.cso");
    }

    void DemoParticles::MengerRenderer::createWindowSizeDependentResources()
    {
        //NOTHING TO DO
    }

    void DemoParticles::MengerRenderer::releaseDeviceDependentResources()
    {
        m_constantBuffer.Reset();
    }

    void DemoParticles::MengerRenderer::update(DX::StepTimer const& /*timer*/, Camera* /*camera*/ /*= nullptr*/)
    {
        
    }

    void DemoParticles::MengerRenderer::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        // Each vertex is one instance of the VertexPositionUV struct.
        UINT stride = (UINT)m_quad->getVertexStride();
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_quad->getMesh(0)->getVertexBuffer().GetAddressOf(), &stride, &offset);

        context->IASetIndexBuffer(m_quad->getMesh(0)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->IASetInputLayout(m_mengerShaderVS->getInputLayout());

        context->VSSetShader(m_mengerShaderVS->getVertexShader(), nullptr, 0);
        context->PSSetShader(m_mengerShaderPS->getPixelShader(), nullptr, 0);

        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
        context->RSSetState(RenderStatesHelper::CullNone().Get());
        context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);

        context->DrawIndexed(m_quad->getMesh(0)->getIndexCount(), 0, 0);
    }
}