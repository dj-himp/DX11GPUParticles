#include "pch.h"
#include "SceneMenger.h"

#include "Camera/CameraControllerFPS.h"
#include "Camera/Camera.h"
#include "Content/MengerRenderer.h"
#include "Common/RenderTarget.h"
#include "Content/BakeModelParticles.h"
#include "Content/RenderFullscreenQuad.h"
#include "Content/RenderParticles.h"
#include "Common/ComputeShader.h"
#include "Common/InputManager.h"
#include "Content/RenderModel.h"

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    SceneMenger::SceneMenger(const DX::DeviceResources* deviceResources)
        : IScene(deviceResources)
    {
        //m_mengerRenderer = std::make_unique<MengerRenderer>(deviceResources);
        //m_bakeModelParticles = std::make_unique<BakeModelParticles>(deviceResources);
        m_fullScreenQuad = std::make_unique<RenderFullscreenQuad>(deviceResources);
        m_renderParticles = std::make_unique<RenderParticles>(deviceResources);
        m_renderModel = std::make_unique<RenderModel>(deviceResources);
    }

    SceneMenger::~SceneMenger()
    {
    }

    void SceneMenger::createDeviceDependentResources()
    {
        float width = 1024.0f;
        float height = 720.0f;
        m_bakingViewport = CD3D11_VIEWPORT(0.0f, 0.0f, width, height);

        m_rtBakePositions = std::make_unique<RenderTarget>(m_deviceResources, DXGI_FORMAT_R16G16B16A16_FLOAT, (UINT)width, (UINT)height);
        m_rtBakeNormals = std::make_unique<RenderTarget>(m_deviceResources, DXGI_FORMAT_R16G16B16A16_FLOAT, (UINT)width, (UINT)height);

        m_computePackParticle = std::make_unique<ComputeShader>(m_deviceResources);// , 2, true);
        m_computePackParticle->load(L"PackParticles_CS.cso");

        CD3D11_BUFFER_DESC desc(sizeof(m_sceneConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&desc, nullptr, &m_sceneConstantBuffer)
        );

        //baking

        m_maxBakeBufferSize = (int)(width * height);

        D3D11_BUFFER_DESC bakeBufferDesc;
        bakeBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        bakeBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        bakeBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        bakeBufferDesc.ByteWidth = sizeof(BackedParticle) * m_maxBakeBufferSize;
        bakeBufferDesc.StructureByteStride = sizeof(BackedParticle);
        bakeBufferDesc.CPUAccessFlags = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&bakeBufferDesc, nullptr, &m_backedBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC bakeUAVDesc;
        bakeUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        bakeUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        bakeUAVDesc.Buffer.FirstElement = 0;
        bakeUAVDesc.Buffer.NumElements = m_maxBakeBufferSize;
        bakeUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_backedBuffer.Get(), &bakeUAVDesc, &m_bakedUAV)
        );


        //indirect compute Args
        D3D11_BUFFER_DESC indirectComputeArgsBufferDesc;
        indirectComputeArgsBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        indirectComputeArgsBufferDesc.ByteWidth = 3 * sizeof(UINT);
        indirectComputeArgsBufferDesc.StructureByteStride = sizeof(UINT);
        indirectComputeArgsBufferDesc.CPUAccessFlags = 0;
        indirectComputeArgsBufferDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        indirectComputeArgsBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&indirectComputeArgsBufferDesc, nullptr, &m_indirectComputeArgsBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC indirectComputeArgsUAVDesc;
        indirectComputeArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        indirectComputeArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        indirectComputeArgsUAVDesc.Buffer.FirstElement = 0;
        indirectComputeArgsUAVDesc.Buffer.NumElements = 3;
        indirectComputeArgsUAVDesc.Buffer.Flags = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_indirectComputeArgsBuffer.Get(), &indirectComputeArgsUAVDesc, &m_indirectComputeArgsUAV)
        );

        CD3D11_BUFFER_DESC indirectArgsDesc(sizeof(InitIndirectComputeArgs1DConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&indirectArgsDesc, nullptr, &m_indirectComputeConstantBuffer)
        );

        m_initIndirectComputeArgsShader = std::make_unique<ComputeShader>(m_deviceResources);
        m_initIndirectComputeArgsShader->load(L"InitIndirectComputeArgs1D_CS.cso");

        m_renderParticles->setBakedIndirectArgs(m_indirectComputeArgsBuffer);
        m_renderParticles->setBakedParticleUAV(m_bakedUAV);

        //m_mengerRenderer->createDeviceDependentResources();
        //m_bakeModelParticles->createDeviceDependentResources();
        m_fullScreenQuad->createDeviceDependentResources();
        m_renderParticles->createDeviceDependentResources();
        m_renderModel->createDeviceDependentResources();
    }

    void SceneMenger::createWindowSizeDependentResources()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
        context->RSSetViewports(1, &viewport);

        Matrix posScale = Matrix::CreateTranslation(0.0f, 0.0f, 0.0f) * Matrix::CreateScale(0.5f, 0.5f, 1.0f);
        m_fullScreenQuad->setPosScale(posScale);

        //m_mengerRenderer->createWindowSizeDependentResources();
        //m_bakeModelParticles->createWindowSizeDependentResources();
        m_fullScreenQuad->createWindowSizeDependentResources();
        m_renderParticles->createWindowSizeDependentResources();
        m_renderModel->createWindowSizeDependentResources();
    }

    void SceneMenger::releaseDeviceDependentResources()
    {
        //m_mengerRenderer->releaseDeviceDependentResources();
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

        Vector4 sunDirection = Vector4(0.5f, -0.5f, -0.5f, 1.0f);
        sunDirection.Normalize();
        m_sceneConstantBufferData.sunDirection = sunDirection;
        m_sceneConstantBufferData.sunColor = Vector4(1.0f, 0.6f, 0.05f, 3.0f);
        m_sceneConstantBufferData.sunSpecColor = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
        m_sceneConstantBufferData.sceneAmbientPower = 0.1f;

        m_sceneConstantBufferData.time = (float)timer.GetTotalSeconds();
        
        static bool started = true;
        if (InputManager::isKeyDown(Keyboard::B))
        {
            started = true;
        }
        if (InputManager::isKeyDown(Keyboard::C))
        {
            started = false;
        }

        m_sceneConstantBufferData.dt = started ? (float)timer.GetElapsedSeconds() : 0.0f;
        m_sceneConstantBufferData.rngSeed = (float)std::rand();


        //m_mengerRenderer->update(timer, camera);
        //m_bakeModelParticles->update(timer);
        m_fullScreenQuad->update(timer);
        m_renderParticles->update(timer, camera);
        m_renderModel->update(timer, camera);
    }

    void SceneMenger::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_sceneConstantBuffer.Get(), 0, nullptr, &m_sceneConstantBufferData, 0, 0);
        context->CSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
        context->VSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
        context->GSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());
        context->PSSetConstantBuffers(0, 1, m_sceneConstantBuffer.GetAddressOf());

        //m_bakingDone = true;
        if (!m_bakingDone)
        {
            //save previously bound renderTargets
            const int nbRT = 2;
            ID3D11RenderTargetView* previousRenderTargets[nbRT];
            ID3D11DepthStencilView* previousDepthStencil;
            context->OMGetRenderTargets(nbRT, previousRenderTargets, &previousDepthStencil);

            ID3D11RenderTargetView* renderTargets[2] = { m_rtBakePositions->getRenderTargetView().Get(), m_rtBakeNormals->getRenderTargetView().Get() };
            context->OMSetRenderTargets(2, renderTargets, nullptr);

            context->RSSetViewports(1, &m_bakingViewport);
            context->ClearRenderTargetView(renderTargets[0], Colors::Transparent);
            context->ClearRenderTargetView(renderTargets[1], Colors::Transparent);

            //m_bakeModelParticles->render();

            D3D11_VIEWPORT viewport = m_deviceResources->GetScreenViewport();
            context->RSSetViewports(1, &viewport);

            context->OMSetRenderTargets(nbRT, previousRenderTargets, previousDepthStencil);

            //OMGetRenderTargets increment the COM refcount so need to release to decrement the count
            //TODO find a better way to handle this
            for (int i = 0; i < nbRT; ++i)
            {
                if (previousRenderTargets[i])
                {
                    previousRenderTargets[i]->Release();
                }
            }
            if (previousDepthStencil)
            {
                previousDepthStencil->Release();
            }

            m_computePackParticle->begin();
            m_computePackParticle->setSRV(0, m_rtBakePositions->getShaderResourceView());
            m_computePackParticle->setSRV(1, m_rtBakeNormals->getShaderResourceView());
            UINT initialCount[] = { 0 };
            m_computePackParticle->setUAV(0, m_bakedUAV, initialCount);
            m_computePackParticle->setUAV(1, m_indirectComputeArgsUAV);
            m_computePackParticle->start(32, 22, 1);
            m_computePackParticle->end();
            m_computePackParticle->setSRV(0, nullptr);
            m_computePackParticle->setSRV(1, nullptr);
            m_computePackParticle->setUAV(0, nullptr);
            m_computePackParticle->setUAV(1, nullptr);

            InitIndirectComputeArgs1DConstantBuffer initBufferData;
            initBufferData.nbThreadGroupX = 256; //see in emit compute shader
            context->UpdateSubresource(m_indirectComputeConstantBuffer.Get(), 0, nullptr, &initBufferData, 0, 0);


            m_initIndirectComputeArgsShader->begin();
            m_initIndirectComputeArgsShader->setConstantBuffer(4, m_indirectComputeConstantBuffer);
            m_initIndirectComputeArgsShader->setUAV(0, m_indirectComputeArgsUAV);
            m_initIndirectComputeArgsShader->start(1, 1, 1);
            m_initIndirectComputeArgsShader->end();
            m_initIndirectComputeArgsShader->setUAV(0, nullptr);

            //m_renderParticles->setBakedIndirectArgs(m_indirectComputeArgsBuffer);
            //m_renderParticles->setBakedParticleUAV(m_bakedUAV);

            m_bakingDone = true;
        }

        //m_mengerRenderer->render();
        m_renderParticles->render();
        //m_renderForceField->render();

        //m_fullScreenQuad->setTexture(m_computePackParticle->getRenderTarget(0)->getShaderResourceView());
        //m_fullScreenQuad->setTexture(m_rtBakePositions->getShaderResourceView().Get());
        //m_fullScreenQuad->render();

        m_renderModel->render();
    }

    void SceneMenger::RenderImGui(Camera* camera)
    {
        m_renderParticles->RenderImGui(camera);
    }

    void SceneMenger::save(json& file)
    {
        m_renderParticles->save(file);
    }

    void SceneMenger::load(json& file)
    {
        m_renderParticles->load(file);
    }

    //TEMP
    int SceneMenger::getCurrentParticlesCount()
    {
        return m_renderParticles->getCurrentParticlesCount();
    }

}