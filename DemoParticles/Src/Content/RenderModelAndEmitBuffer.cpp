#include "pch.h"
#include "RenderModelAndEmitBuffer.h"

#include "Model/ModelLoader.h"
#include "Model/Model.h"
#include "Camera/Camera.h"
#include "Common/InputManager.h"
#include "Common/VertexShader.h"
#include "Common/PixelShader.h"
#include "Common/GeometryShader.h"
#include "Common/ComputeShader.h"

#include <stdlib.h>
#include <time.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderModelAndEmitBuffer::RenderModelAndEmitBuffer(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        m_modelToEmitConstantBufferData.scaleDensity = DirectX::SimpleMath::Vector2(1.0f, 1.0f);
        m_modelToEmitConstantBufferData.offsetDensity = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
        m_scaleDensity = DirectX::SimpleMath::Vector2(0.005f, 0.005f);
        m_offsetDensity = DirectX::SimpleMath::Vector2(0.0f, 0.0f);
        srand(time(NULL));
    }

    void RenderModelAndEmitBuffer::createDeviceDependentResources()
    {
        m_modelLoader = std::make_unique<ModelLoader>(m_deviceResources);
        //m_model = m_modelLoader->load("stanford-bunny.fbx");
        //m_model = m_modelLoader->load("cube.dae");
        m_model = m_modelLoader->load("Blender_Monkey_Suzanne.fbx");
        //m_model = m_modelLoader->load("Wolf.fbx");

        m_modelVS = std::make_unique<VertexShader>(m_deviceResources);
        m_modelVS->load(L"RenderModelAndEmit_VS.cso", m_model->getInputElements());

        m_modelPS = std::make_unique<PixelShader>(m_deviceResources);
        m_modelPS->load(L"RenderModelAndEmit_PS.cso");

        m_modelGS = std::make_unique<GeometryShader>(m_deviceResources);
        m_modelGS->load(L"RenderModelAndEmit_GS.cso");

        m_modelResetArgsCS = std::make_unique<ComputeShader>(m_deviceResources);
        m_modelResetArgsCS->load(L"RenderModelAndEmitInit_CS.cso");

        m_modelInitArgsCS = std::make_unique<ComputeShader>(m_deviceResources);
        m_modelInitArgsCS->load(L"InitIndirectComputeArgs1D_CS.cso");

        m_modelEmitCS = std::make_unique<ComputeShader>(m_deviceResources);
        m_modelEmitCS->load(L"EmitParticlesFromAppendBuffer_CS.cso");

        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(m_constantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &constantBufferDesc,
                nullptr,
                &m_constantBuffer
            )
        );

        CD3D11_BUFFER_DESC modelToEmitconstantBufferDesc(sizeof(m_modelToEmitConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &modelToEmitconstantBufferDesc,
                nullptr,
                &m_modelToEmitConstantBuffer
            )
        );

        int maxParticles = 1000000; //TEMP
        CD3D11_BUFFER_DESC particlesBufferDesc;
        particlesBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        particlesBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        particlesBufferDesc.CPUAccessFlags = 0;
        particlesBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        particlesBufferDesc.ByteWidth = sizeof(Particle) * maxParticles;
        particlesBufferDesc.StructureByteStride = sizeof(Particle);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&particlesBufferDesc, nullptr, &m_particleBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC particlesUAVDesc;
        particlesUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particlesUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        particlesUAVDesc.Buffer.FirstElement = 0;
        particlesUAVDesc.Buffer.NumElements = maxParticles;
        particlesUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_particleBuffer.Get(), &particlesUAVDesc, &m_particleUAV)
        );


        D3D11_BUFFER_DESC indirectDrawArgsBuffer;
        ZeroMemory(&indirectDrawArgsBuffer, sizeof(indirectDrawArgsBuffer));
        indirectDrawArgsBuffer.Usage = D3D11_USAGE_DEFAULT;
        indirectDrawArgsBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        indirectDrawArgsBuffer.ByteWidth = 3 * sizeof(UINT);
        indirectDrawArgsBuffer.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&indirectDrawArgsBuffer, nullptr, &m_indirectDispatchArgsBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDrawArgsUAVDesc;
        indirectDrawArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        indirectDrawArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        indirectDrawArgsUAVDesc.Buffer.FirstElement = 0;
        indirectDrawArgsUAVDesc.Buffer.NumElements = 3;
        indirectDrawArgsUAVDesc.Buffer.Flags = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_indirectDispatchArgsBuffer.Get(), &indirectDrawArgsUAVDesc, &m_indirectDispatchArgsUAV)
        );


        //verify if this is the same number of threads in the destination shader
        m_indirectDispatchArgsConstantBufferData.nbThreadGroupX = 256.0f;

        CD3D11_BUFFER_DESC indirectDispatchArgsConstantBufferDesc(sizeof(m_indirectDispatchArgsConstantBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indirectDispatchArgsConstantBufferDesc,
                nullptr,
                &m_indirectDispatchArgsConstantBuffer
            )
        );

        m_deviceResources->GetD3DDeviceContext()->UpdateSubresource1(m_indirectDispatchArgsConstantBuffer.Get(), 0, NULL, &m_indirectDispatchArgsConstantBufferData, 0, 0, 0);


        //Z rotation is temporary as I need to know why the model is upside down
        //m_world = Matrix::CreateScale(0.001f) * Matrix::CreateRotationX(0.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(DirectX::XM_PI) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
        m_world = Matrix::CreateScale(0.01f) * Matrix::CreateRotationX(-DirectX::XM_PI / 2.0f) * Matrix::CreateRotationY(0.0f/*DirectX::XM_PI / 2.0f*/) * Matrix::CreateRotationZ(/*DirectX::XM_PI*/0.0f) * Matrix::CreateTranslation(0.0f, 0.0f, 0.0f);
    }

    void RenderModelAndEmitBuffer::createWindowSizeDependentResources()
    {
        
    }

    void RenderModelAndEmitBuffer::releaseDeviceDependentResources()
    {
       //TO DO
    }

    void RenderModelAndEmitBuffer::update(DX::StepTimer const& timer, Camera* /*camera*/ /*= nullptr*/)
    {
        //assert(camera);

        XMStoreFloat4x4(&m_constantBufferData.world, m_world.Transpose());

        float rnd = (1 - (rand() % 2)) / 2.0f;
        m_modelToEmitConstantBufferData.offsetDensity = m_offsetDensity * rnd * m_scaleDensity;
        m_modelToEmitConstantBufferData.scaleDensity = m_scaleDensity;
    }

    void RenderModelAndEmitBuffer::render()
    {

        auto context = m_deviceResources->GetD3DDeviceContext();
        
        //reset indirect dispatch args

        m_modelResetArgsCS->setUAV(0, m_indirectDispatchArgsUAV);
        m_modelResetArgsCS->begin();
        m_modelResetArgsCS->start(1, 1, 1);
        m_modelResetArgsCS->end();
        m_modelResetArgsCS->setUAV(0, nullptr);

        //render mesh + add particles to append buffer
        context->UpdateSubresource1(m_constantBuffer.Get(), 0, NULL, &m_constantBufferData, 0, 0, 0);
        context->UpdateSubresource1(m_modelToEmitConstantBuffer.Get(), 0, NULL, &m_modelToEmitConstantBufferData, 0, 0, 0);
        
        //begin at 2 because we have 1 RT&Depth and UAVs slot start after RTs
        UINT initialCounts[] = { 0, (UINT)-1 };
        ID3D11UnorderedAccessView* uavs[] = { m_particleUAV.Get(), m_indirectDispatchArgsUAV.Get() };
        m_modelPS->setUAVs(2, 2, uavs, initialCounts);

        //only first model for t-rex because all the same
        //int i = 0;
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
            
            context->GSSetShader(m_modelGS->getGeometryShader(), nullptr, 0);
            context->GSSetConstantBuffers(1, 1, m_modelToEmitConstantBuffer.GetAddressOf());

            context->RSSetState(RenderStatesHelper::CullCounterClockwise().Get());


            context->PSSetShader(m_modelPS->getPixelShader(), nullptr, 0);

            context->DrawIndexed(m_model->getMesh(i)->getIndexCount(), 0, 0);

            //int count = m_modelPS->readCounter(m_particleUAV);

            context->GSSetShader(nullptr, nullptr, 0);
            
        }

        ZeroMemory(uavs, sizeof(uavs));
        m_modelPS->setUAVs(2, 2, uavs);

        context->CSSetConstantBuffers(4, 1, m_indirectDispatchArgsConstantBuffer.GetAddressOf());
        //init dispatchIndirect args
        m_modelInitArgsCS->setUAV(0, m_indirectDispatchArgsUAV);
        m_modelInitArgsCS->begin();
        m_modelInitArgsCS->start(1, 1, 1);
        m_modelInitArgsCS->end();
        m_modelInitArgsCS->setUAV(0, nullptr);
    }


    void RenderModelAndEmitBuffer::RenderImGui(Camera* camera)
    {
        ImGui::DragFloat2("Scale density", (float*)&m_scaleDensity, 0.01f, 0.01f, 10.0f);
        ImGui::DragFloat2("Offset density", (float*)&m_offsetDensity, 0.01f, 0.01f, 10.0f);
    }
}