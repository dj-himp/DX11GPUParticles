#include "pch.h"
#include "AxisRenderer.h"

#include "Model/MeshFactory.h"
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

        m_world = Matrix::CreateScale(1.0f);
    }

    void AxisRenderer::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void AxisRenderer::createDeviceDependentResources()
    {
        
        //MeshFactory meshCreator(m_deviceResources);
        //m_axis = meshCreator.createAxis();

        m_axis = MeshFactory::getInstance().createAxis();

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderDebugColor_VS.cso", L"RenderDebugColor_PS.cso", m_axis->getInputElements());

        

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(m_constantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );
    }

    void AxisRenderer::createWindowSizeDependentResources()
    {
        
    }

    void AxisRenderer::releaseDeviceDependentResources()
    {
        
    }

    void AxisRenderer::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        assert(camera);
        
        m_constantBufferData.world = m_world.Transpose();
    }

    void AxisRenderer::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);

        for (int i = 0; i < m_axis->getMeshCount(); ++i)
        {
            UINT stride = m_axis->getVertexStride(); //sizeof(VertexColorUV);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_axis->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_axis->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(m_axis->getMesh(i)->getPrimitiveTopology());
            context->IASetInputLayout(m_shader->getInputLayout());

            context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());

            const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
            context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
            context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);
            context->RSSetState(RenderStatesHelper::CullNone().Get());

            context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_axis->getMesh(i)->getIndexCount(), 0, 0);
        }
    }
}
