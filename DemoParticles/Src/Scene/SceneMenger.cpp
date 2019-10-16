#include "pch.h"
#include "SceneMenger.h"

#include "Common/DirectXHelper.h"
#include "Camera/CameraControllerFPS.h"
#include "Camera/Camera.h"
#include "Content/MengerRenderer.h"
#include "Common/RenderTarget.h"
#include "Content/BakeModelParticles.h"
#include "Content/RenderFullscreenQuad.h"
#include "Content/RenderParticles.h"

#include "Common/ComputeShader.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    SceneMenger::SceneMenger(const DX::DeviceResources* deviceResources)
        : IScene(deviceResources)
    {
        m_mengerRenderer = std::make_unique<MengerRenderer>(deviceResources);
        m_bakeModelParticles = std::make_unique<BakeModelParticles>(deviceResources);
        m_fullScreenQuad = std::make_unique<RenderFullscreenQuad>(deviceResources);
        m_renderParticles = std::make_unique<RenderParticles>(deviceResources);

        createDeviceDependentResources();
        createWindowSizeDependentResources();
    }

    SceneMenger::~SceneMenger()
    {
    }

    void SceneMenger::createDeviceDependentResources()
    {
        m_mengerRenderer->createDeviceDependentResources();
        m_renderParticles->createDeviceDependentResources();

        m_rtBakePositions = std::make_unique<RenderTarget>(m_deviceResources, DXGI_FORMAT_R16G16B16A16_FLOAT, m_deviceResources->GetOutputWidth(), m_deviceResources->GetOutputHeight());
        m_rtBakeNormals = std::make_unique<RenderTarget>(m_deviceResources, DXGI_FORMAT_R16G16B16A16_FLOAT, m_deviceResources->GetOutputWidth(), m_deviceResources->GetOutputHeight());

        m_computePackParticle = std::make_unique<ComputeShader>(m_deviceResources);// , 2, true);
        m_computePackParticle->load(L"PackParticles_CS.cso");

        CD3D11_BUFFER_DESC desc(sizeof(m_sceneConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&desc, nullptr, &m_sceneConstantBuffer)
        );
    }

    void SceneMenger::createWindowSizeDependentResources()
    {
        Matrix posScale = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f) * Matrix::CreateScale(0.5f, 0.5f, 1.0f);
        m_fullScreenQuad->setPosScale(posScale);

        m_mengerRenderer->createWindowSizeDependentResources();

    }

    void SceneMenger::releaseDeviceDependentResources()
    {
        m_mengerRenderer->releaseDeviceDependentResources();
    }

    void SceneMenger::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        m_sceneConstantBufferData.view = camera->getView().Transpose();
        m_sceneConstantBufferData.projection = camera->getProjection().Transpose();
        Matrix viewProj = camera->getView() * camera->getProjection();
        m_sceneConstantBufferData.viewProj = viewProj.Transpose();
        
        std::vector<Vector3> corners = camera->getFrustrumCorners();
        m_sceneConstantBufferData.frustumCorner[0] = DX::toVector4(corners[4]);
        m_sceneConstantBufferData.frustumCorner[1] = DX::toVector4(corners[5]);
        m_sceneConstantBufferData.frustumCorner[2] = DX::toVector4(corners[6]);
        m_sceneConstantBufferData.frustumCorner[3] = DX::toVector4(corners[7]);
        
        m_sceneConstantBufferData.camPosition = DX::toVector4(camera->getPosition());
        m_sceneConstantBufferData.camDirection = DX::toVector4(camera->getForward());
        m_sceneConstantBufferData.time = timer.GetTotalSeconds();
        m_sceneConstantBufferData.dt = timer.GetElapsedSeconds();

        m_mengerRenderer->update(timer, camera);
        m_bakeModelParticles->update(timer);
        m_fullScreenQuad->update(timer);
        m_renderParticles->update(timer, camera);
    }

    void SceneMenger::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_sceneConstantBuffer.Get(), 0, nullptr, &m_sceneConstantBufferData, 0, 0);
        context->CSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
        context->VSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
        context->GSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());

        if (!m_bakingDone)
        {
            //save previously bound renderTargets
            const int nbRT = 2;
            ID3D11RenderTargetView* previousRenderTargets[nbRT];
            ID3D11DepthStencilView* previousDepthStencil;
            context->OMGetRenderTargets(nbRT, previousRenderTargets, &previousDepthStencil);

            ID3D11RenderTargetView* renderTargets[2] = { m_rtBakePositions->getRenderTargetView().Get(), m_rtBakeNormals->getRenderTargetView().Get() };
            context->OMSetRenderTargets(2, renderTargets, nullptr);

            context->ClearRenderTargetView(renderTargets[0], Colors::Transparent);
            context->ClearRenderTargetView(renderTargets[1], Colors::Transparent);

            m_bakeModelParticles->render();

            context->OMSetRenderTargets(nbRT, previousRenderTargets, previousDepthStencil);

            m_computePackParticle->begin();
            m_computePackParticle->setSRV(0, m_rtBakePositions->getShaderResourceView());
            m_computePackParticle->setSRV(1, m_rtBakeNormals->getShaderResourceView());
            m_computePackParticle->start(32, 22, 1);
            m_computePackParticle->end();
            m_computePackParticle->setSRV(0, nullptr);
            m_computePackParticle->setSRV(1, nullptr);

            m_bakingDone = true;
        }

        //m_mengerRenderer->render();

        m_renderParticles->render();

        //m_fullScreenQuad->setTexture(m_computePackParticle->getRenderTarget(0)->getShaderResourceView());
        //m_fullScreenQuad->setTexture(m_rtBakePositions->getShaderResourceView().Get());
        //m_fullScreenQuad->render();


    }
}