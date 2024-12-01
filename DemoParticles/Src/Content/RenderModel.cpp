#include "pch.h"
#include "RenderModel.h"

#include "Model/ModelLoader.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/InputManager.h"
#include "Common/VertexShader.h"
#include "Common/PixelShader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderModel::RenderModel(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {

    }

    void RenderModel::createDeviceDependentResources()
    {
        m_modelLoader = std::make_unique<ModelLoader>(m_deviceResources);
        //m_model = m_modelLoader->load("CatMac.fbx");
        //m_model = m_modelLoader->load("deer.fbx");
        m_model = m_modelLoader->load("magician.X");

        m_modelVS = std::make_unique<VertexShader>(m_deviceResources);
        m_modelVS->load(L"RenderModel_VS.cso", m_model->getInputElements());

        m_modelPS = std::make_unique<PixelShader>(m_deviceResources);
        m_modelPS->load(L"RenderModel_PS.cso");

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(m_constantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );

		CD3D11_BUFFER_DESC debugConstantBufferDesc(sizeof(m_debugConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&debugConstantBufferDesc,
				nullptr,
				&m_debugConstantBuffer
			)
		);

		CD3D11_BUFFER_DESC skinnedConstantBufferDesc(sizeof(m_skinnedConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
		DX::ThrowIfFailed(
			m_deviceResources->GetD3DDevice()->CreateBuffer(
				&skinnedConstantBufferDesc,
				nullptr,
				&m_skinnedConstantBuffer
			)
		);


        //Z rotation is temporary as I need to know why the model is upside down
        m_world = Matrix::CreateScale(0.001f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(DirectX::XM_PI / 2.0f) * Matrix::CreateRotationZ(-DirectX::XM_PI/2.0f) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }

    void RenderModel::createWindowSizeDependentResources()
    {
        
    }

    void RenderModel::releaseDeviceDependentResources()
    {
       //TO DO
    }

    void RenderModel::update(DX::StepTimer const& /*timer*/, Camera* /*camera*/ /*= nullptr*/)
    {
        //assert(camera);

        XMStoreFloat4x4(&m_constantBufferData.world, m_world.Transpose());

        if (InputManager::isKeyDown(Keyboard::A))
        {
            m_debugConstantBufferData.boneID = (m_debugConstantBufferData.boneID+1) % m_model->m_Bones.size();
            Sleep(100);
        }

		std::vector<DirectX::SimpleMath::Matrix> Transforms;
		m_model->GetBoneTransforms(0.0f, Transforms);

		for (int i = 0; i < Transforms.size(); ++i)
        {
            //m_skinnedConstantBufferData.boneTransforms[i] = Transforms[i].Transpose();
            m_skinnedConstantBufferData.boneTransforms[i] = Transforms[i];
		}
    }

    void RenderModel::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();
        
        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
        context->UpdateSubresource1(m_debugConstantBuffer.Get(), 0, NULL, &m_debugConstantBufferData, 0, 0, 0);
        context->UpdateSubresource1(m_skinnedConstantBuffer.Get(), 0, NULL, &m_skinnedConstantBufferData, 0, 0, 0);
        
        for (int i = 0; i < m_model->getMeshCount(); ++i)
        {
            UINT stride = (UINT)m_model->getVertexStride();//sizeof(VertexObject);
            UINT offset = 0;
            context->IASetVertexBuffers(0, 1, m_model->getMesh(i)->getVertexBuffer().GetAddressOf(), &stride, &offset);
            context->IASetIndexBuffer(m_model->getMesh(i)->getIndexBuffer().Get(), DXGI_FORMAT_R32_UINT, 0);

            context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
            context->IASetInputLayout(m_modelVS->getInputLayout());

            context->VSSetShader(m_modelVS->getVertexShader(), nullptr, 0);
            context->VSSetConstantBuffers(1, 1, m_constantBuffer.GetAddressOf());
            context->VSSetConstantBuffers(2, 1, m_debugConstantBuffer.GetAddressOf());
            context->VSSetConstantBuffers(3, 1, m_skinnedConstantBuffer.GetAddressOf());

            context->RSSetState(RenderStatesHelper::CullCounterClockwise().Get());

            context->PSSetShader(m_modelPS->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_model->getMesh(i)->getIndexCount(), 0, 0);
        }
    }

    void RenderModel::updateConstantBuffer()
    {
        XMStoreFloat4x4(&m_constantBufferData.world, m_world.Transpose());
    }

}