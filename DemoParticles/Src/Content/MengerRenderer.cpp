#include "pch.h"
#include "MengerRenderer.h"

#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/Shader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    MengerRenderer::MengerRenderer(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }

    void DemoParticles::MengerRenderer::init()
    {
        IRenderable::init();
    }

    void DemoParticles::MengerRenderer::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void DemoParticles::MengerRenderer::createDeviceDependentResources()
    {
        
        //m_meshFactory = std::make_unique<MeshFactory>(m_deviceResources);
        //m_quad = m_meshFactory->createQuad();

        m_quad = MeshFactory::getInstance().createQuad();

        m_shader = std::make_unique<Shader>(m_deviceResources);
        //m_shader->load(L"RenderDebugRM_VS.cso", L"RenderDebugRM_PS.cso", m_quad->getInputElements());
        m_shader->load(L"RenderMenger_VS.cso", L"RenderMenger_PS.cso", m_quad->getInputElements());
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

        context->IASetInputLayout(m_shader->getInputLayout());

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);

        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
        context->RSSetState(RenderStatesHelper::CullNone().Get());
        context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);


        context->DrawIndexed(m_quad->getMesh(0)->getIndexCount(), 0, 0);
    }
}