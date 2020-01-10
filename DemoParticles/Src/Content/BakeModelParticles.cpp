#include "pch.h"
#include "BakeModelParticles.h"

#include "Model/ModelLoader.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/InputManager.h"
#include "Common/Shader.h"
#include "Common/RenderTarget.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    BakeModelParticles::BakeModelParticles(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
       
    }

    void BakeModelParticles::createDeviceDependentResources()
    {
        m_modelLoader = std::make_unique<ModelLoader>(m_deviceResources);
        //m_model = m_modelLoader->load("CatMac.fbx");
        m_model = m_modelLoader->load("TrexByJoel3d.fbx");
        //m_model = m_modelLoader->load("deer.fbx");

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"BakeObjectParticles_VS.cso", L"BakeObjectParticles_PS.cso", m_model->getInputElements());

        
        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(WorldConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );

        //Z rotation is temporary as I need to know why the model is upside down
        //m_world = Matrix::CreateScale(0.1f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
        m_world = Matrix::CreateScale(0.5f, -0.5f, 0.5f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
        //m_world = Matrix::CreateScale(1.0f, 1.0f, 1.0f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(0.0f) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }

    void BakeModelParticles::createWindowSizeDependentResources()
    {
        
    }

    void BakeModelParticles::releaseDeviceDependentResources()
    {
       //TO DO
    }

    void BakeModelParticles::update(DX::StepTimer const& /*timer*/, Camera* /*camera*/ /*= nullptr*/)
    {
        m_constantBufferData.world = m_world.Transpose();
    }

    void BakeModelParticles::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();
        
        context->UpdateSubresource(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0);
        
        for (int i = 0; i < m_model->getMeshCount(); ++i)
        {
            UINT stride = (UINT)m_model->getVertexStride();
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->IASetInputLayout(m_shader->getInputLayout());

            context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());

            context->RSSetState(RenderStatesHelper::CullNone().Get());

            context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_model->getMesh(i)->getIndexCount(), 0, 0);

            
        }
    }
}