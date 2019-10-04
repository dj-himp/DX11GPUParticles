#include "pch.h"
#include "MengerRenderer.h"

#include "Common/DirectXHelper.h"
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
        

        CD3D11_BUFFER_DESC constantBufferDescVS(sizeof(FrustrumCornersConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDescVS,
                nullptr,
                &m_constantBufferVS
            )
        );

        CD3D11_BUFFER_DESC constantBufferDescPS(sizeof(MengerPSConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDescPS,
                nullptr,
                &m_constantBufferPS
            )
        );
    }

    void DemoParticles::MengerRenderer::createWindowSizeDependentResources()
    {
        //NOTHING TO DO
    }

    void DemoParticles::MengerRenderer::releaseDeviceDependentResources()
    {
        m_constantBufferVS.Reset();
    }

    void DemoParticles::MengerRenderer::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        std::vector<Vector3> corners = camera->getFrustrumCorners();
        m_constantBufferDataVS.frustumCorner[0] = DX::toVector4(corners[4]);
        m_constantBufferDataVS.frustumCorner[1] = DX::toVector4(corners[5]);
        m_constantBufferDataVS.frustumCorner[2] = DX::toVector4(corners[6]);
        m_constantBufferDataVS.frustumCorner[3] = DX::toVector4(corners[7]);
        
        m_constantBufferDataPS.camPosition = DX::toVector4(camera->getPosition());
        m_constantBufferDataPS.camDirection = DX::toVector4(camera->getForward());
        m_constantBufferDataPS.time = timer.GetTotalSeconds();
    }

    void DemoParticles::MengerRenderer::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        // Prepare the constant buffer to send it to the graphics device.
        context->UpdateSubresource1(m_constantBufferVS.Get(), 0, NULL, &m_constantBufferDataVS, 0, 0, 0);
        context->UpdateSubresource1(m_constantBufferPS.Get(), 0, NULL, &m_constantBufferDataPS, 0, 0, 0);

        // Each vertex is one instance of the VertexPositionUV struct.
        UINT stride = m_quad->getVertexStride(); //sizeof(VertexColorUV);
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_quad->getMesh(0)->getVertexBuffer().GetAddressOf(), &stride, &offset);

        context->IASetIndexBuffer(m_quad->getMesh(0)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        context->IASetInputLayout(m_shader->getInputLayout());

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
        context->VSSetConstantBuffers1(0, 1, m_constantBufferVS.GetAddressOf(), nullptr, nullptr);

        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);
        context->PSSetConstantBuffers1(0, 1, m_constantBufferPS.GetAddressOf(), nullptr, nullptr);

        context->DrawIndexed(m_quad->getMesh(0)->getIndexCount(), 0, 0);
    }
}