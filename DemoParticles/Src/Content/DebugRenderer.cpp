#include "pch.h"
#include "DebugRenderer.h"

#include "Model/MeshFactory.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/Shader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    /*DebugRenderer::DebugRenderer(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }*/

    DebugRenderer& DebugRenderer::instance()
    {
        static DebugRenderer instance = DebugRenderer();
        return instance;
    }

    void DebugRenderer::init()
    {

    }

    void DebugRenderer::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void DebugRenderer::createDeviceDependentResources()
    {
        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderDebugColor_VS.cso", L"RenderDebugColor_PS.cso", MeshFactory::getInstance().getVertexElements());

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(WorldConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );

        D3D11_BLEND_DESC blendDesc;
        ZeroMemory(&blendDesc, sizeof(D3D11_BLEND_DESC));
        blendDesc.RenderTarget[0].BlendEnable = FALSE;
        //blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
    }

    void DebugRenderer::createWindowSizeDependentResources()
    {
        
    }

    void DebugRenderer::releaseDeviceDependentResources()
    {
        
    }

    void DebugRenderer::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        assert(camera);
        
        //Matrix m_world = Matrix::Identity;
        //m_constantBufferData.world = m_world.Transpose();
    }

    void DebugRenderer::render()
    {
        for (auto& debugModel : m_models)
        {
            m_constantBufferData.world = debugModel.m_world.Transpose();
            renderDebugModel(debugModel);
        }
    }

    DebugRenderer::DebugRenderer()
    {

    }

    void DebugRenderer::renderDebugModel(DebugModel& debugModel)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);

        for (int i = 0; i < debugModel.m_model->getMeshCount(); ++i)
        {
            UINT stride = debugModel.m_model->getVertexStride(); //sizeof(VertexColorUV);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, debugModel.m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(debugModel.m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(debugModel.m_model->getMesh(i)->getPrimitiveTopology());
            context->IASetInputLayout(m_shader->getInputLayout());

            context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());

            context->RSSetState(RenderStatesHelper::CullNone().Get());

            context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

            ID3D11RenderTargetView* renderTargets[1] = { m_deviceResources->GetRenderTargetView() };
            context->OMSetRenderTargets(1, renderTargets, m_deviceResources->GetDepthStencilView());

            context->DrawIndexed(debugModel.m_model->getMesh(i)->getIndexCount(), 0, 0);
        }
    }

    void DebugRenderer::pushBackModel(std::unique_ptr<Model> model, DirectX::SimpleMath::Matrix world /*= DirectX::SimpleMath::Matrix::Identity*/)
    {
        /*DebugModel debugModel;
        debugModel.m_model = std::move(model);
        debugModel.m_world = world;*/

        //m_models.push_back(debugModel);
        m_models.emplace_back(std::move(model), world );
    }

}
