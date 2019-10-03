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
        m_shader->load(L"RenderQuad_VS.cso", L"RenderQuad_PS.cso", m_quad->getInputElements());
        

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(QuadConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );
    }

    void DemoParticles::MengerRenderer::createWindowSizeDependentResources()
    {
        //NOTHING TO DO
    }

    void DemoParticles::MengerRenderer::releaseDeviceDependentResources()
    {
        m_constantBuffer.Reset();
    }

    void DemoParticles::MengerRenderer::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        m_posScale = Matrix::CreateTranslation(Vector3(0.0f, 0.0f, 0.0f)) * Matrix::CreateScale(1.0f);
        XMStoreFloat4x4(&m_constantBufferData.posScale, m_posScale.Transpose());
    }

    void DemoParticles::MengerRenderer::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        // Prepare the constant buffer to send it to the graphics device.
        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);

        // Each vertex is one instance of the VertexPositionUV struct.
        UINT stride = m_quad->getVertexStride(); //sizeof(VertexColorUV);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_quad->getMesh(0)->getVertexBuffer().GetAddressOf(), &stride, &offset);

        context->IASetIndexBuffer(m_quad->getMesh(0)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->IASetInputLayout(m_shader->getInputLayout());

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);

        context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

        context->DrawIndexed(m_quad->getMesh(0)->getIndexCount(), 0, 0);
    }
}