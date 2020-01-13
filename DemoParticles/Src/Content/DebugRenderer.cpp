#include "pch.h"
#include "DebugRenderer.h"

#include "Model/MeshFactory.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/VertexShader.h"
#include "Common/PixelShader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    DebugRenderer& DebugRenderer::instance()
    {
        static DebugRenderer instance = DebugRenderer();
        return instance;
    }

    void DebugRenderer::release()
    {
        throw std::logic_error("The method or operation is not implemented.");
    }

    void DebugRenderer::createDeviceDependentResources()
    {
        m_debugVS = std::make_unique<VertexShader>(m_deviceResources);
        m_debugVS->load(L"RenderDebugColor_VS.cso", MeshFactory::getInstance().getVertexElements());

        m_debugPS = std::make_unique<PixelShader>(m_deviceResources);
        m_debugPS->load(L"RenderDebugColor_PS.cso");

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(WorldConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );
    }

    void DebugRenderer::createWindowSizeDependentResources()
    {
        
    }

    void DebugRenderer::releaseDeviceDependentResources()
    {
        
    }

    void DebugRenderer::update(DX::StepTimer const& /*timer*/, Camera* /*camera*/ /*= nullptr*/)
    {
        //assert(camera);
        
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
            UINT stride = (UINT)debugModel.m_model->getVertexStride();
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, debugModel.m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(debugModel.m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(debugModel.m_model->getMesh(i)->getPrimitiveTopology());
            context->IASetInputLayout(m_debugVS->getInputLayout());

            context->VSSetShader(m_debugVS->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());

            const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
            context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
            context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);
            context->RSSetState(RenderStatesHelper::CullNone().Get());

            context->PSSetShader(m_debugPS->getPixelShader(), nullptr, 0);

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
