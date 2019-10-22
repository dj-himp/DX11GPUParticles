#include "pch.h"
#include "RenderParticles.h"

#include "Common/DirectXHelper.h"
#include "Common/Shader.h"
#include "Common/ComputeShader.h"
#include "Camera/Camera.h"
#include "Common/SortLib.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderParticles::RenderParticles(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
    }

    void RenderParticles::init()
    {
        IRenderable::init();
    }

    void RenderParticles::release()
    {
        
    }

    void RenderParticles::createDeviceDependentResources()
    {
        //NEED REFACTOR of Shader::Load to remove this 
        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderParticles_VS.cso", L"RenderParticles_PS.cso", inputElementDesc, L"RenderParticles_GS.cso");
        
        //MAIN PARTICLE POOL

        D3D11_BUFFER_DESC particleBufferDesc;
        particleBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        particleBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        particleBufferDesc.CPUAccessFlags = 0;
        particleBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        particleBufferDesc.ByteWidth = sizeof(Particle) * m_maxParticles;
        particleBufferDesc.StructureByteStride = sizeof(Particle);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&particleBufferDesc, nullptr, &m_particleBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC particleUAVDesc;
        particleUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particleUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        particleUAVDesc.Buffer.FirstElement = 0;
        particleUAVDesc.Buffer.NumElements = m_maxParticles;
        particleUAVDesc.Buffer.Flags = 0;
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_particleBuffer.Get(), &particleUAVDesc, &m_particleUAV)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC particleSRVDesc;
        particleSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        particleSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        particleSRVDesc.Buffer.FirstElement = 0;
        particleSRVDesc.Buffer.NumElements = m_maxParticles;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_particleBuffer.Get(), &particleSRVDesc, &m_particleSRV)
        );

        //DEAD LIST
        D3D11_BUFFER_DESC deadListBufferDesc;
        deadListBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        deadListBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        deadListBufferDesc.CPUAccessFlags = 0;
        deadListBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        deadListBufferDesc.ByteWidth = sizeof(UINT) * m_maxParticles;
        deadListBufferDesc.StructureByteStride = sizeof(UINT);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListBufferDesc, nullptr, &m_deadListBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC deadListUAVDesc;
        deadListUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        deadListUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        deadListUAVDesc.Buffer.FirstElement = 0;
        deadListUAVDesc.Buffer.NumElements = m_maxParticles;
        deadListUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_deadListBuffer.Get(), &deadListUAVDesc, &m_deadListUAV)
        );


        //ALIVE PARTICLE LIST
        D3D11_BUFFER_DESC aliveIndexBufferDesc;
        aliveIndexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
        aliveIndexBufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
        aliveIndexBufferDesc.CPUAccessFlags = 0;
        aliveIndexBufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
        aliveIndexBufferDesc.ByteWidth = sizeof(ParticleIndexElement) * m_maxParticles;
        aliveIndexBufferDesc.StructureByteStride = sizeof(ParticleIndexElement);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&aliveIndexBufferDesc, nullptr, &m_aliveIndexBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC aliveIndexUAVDesc;
        aliveIndexUAVDesc.Format = DXGI_FORMAT_UNKNOWN;
        aliveIndexUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        aliveIndexUAVDesc.Buffer.FirstElement = 0;
        aliveIndexUAVDesc.Buffer.NumElements = m_maxParticles;
        aliveIndexUAVDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_aliveIndexBuffer.Get(), &aliveIndexUAVDesc, &m_aliveIndexUAV)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC aliveIndexSRVDesc;
        aliveIndexSRVDesc.Format = DXGI_FORMAT_UNKNOWN;
        aliveIndexSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
        aliveIndexSRVDesc.Buffer.FirstElement = 0;
        aliveIndexSRVDesc.Buffer.NumElements = m_maxParticles;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_aliveIndexBuffer.Get(), &aliveIndexSRVDesc, &m_aliveIndexSRV)
        );

        m_initDeadListShader = std::make_unique<ComputeShader>(m_deviceResources);
        m_initDeadListShader->load(L"ResetParticles_CS.cso");

        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticles_CS.cso");

        m_emitFromBufferParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitFromBufferParticles->load(L"EmitParticlesFromBuffer_CS.cso");

        //EMITTER CONSTANT BUFFER
        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );

        //EMITTER FROM BUFFER CONSTANT BUFFER
        CD3D11_BUFFER_DESC emitterFromBufferConstantBufferDesc(sizeof(EmitterFromBufferConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterFromBufferConstantBufferDesc, nullptr, &m_emitterFromBufferConstantBuffer)
        );

        //DeadList CONSTANT BUFFER
        CD3D11_BUFFER_DESC deadListCountConstantBufferDesc(sizeof(DeadListCountConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer)
        );
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_aliveListCountConstantBuffer)
        );

        //Indirect Draw Args Buffer
        D3D11_BUFFER_DESC indirectDrawArgsBuffer;
        ZeroMemory(&indirectDrawArgsBuffer, sizeof(indirectDrawArgsBuffer));
        indirectDrawArgsBuffer.Usage = D3D11_USAGE_DEFAULT;
        indirectDrawArgsBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
        indirectDrawArgsBuffer.ByteWidth = 5 * sizeof(UINT);
        indirectDrawArgsBuffer.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&indirectDrawArgsBuffer, nullptr, &m_indirectDrawArgsBuffer)
        );

        D3D11_UNORDERED_ACCESS_VIEW_DESC indirectDrawArgsUAVDesc;
        indirectDrawArgsUAVDesc.Format = DXGI_FORMAT_R32_UINT;
        indirectDrawArgsUAVDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
        indirectDrawArgsUAVDesc.Buffer.FirstElement = 0;
        indirectDrawArgsUAVDesc.Buffer.NumElements = 5;
        indirectDrawArgsUAVDesc.Buffer.Flags = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_indirectDrawArgsBuffer.Get(), &indirectDrawArgsUAVDesc, &m_indirectDrawArgsUAV)
        );

        CD3D11_BUFFER_DESC particlesGlobalSettingsDesc(sizeof(m_particlesGlobalSettingsBufferData), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&particlesGlobalSettingsDesc, nullptr, &m_particlesGlobalSettingsBuffer)
        );

        m_simulateShader = std::make_unique<ComputeShader>(m_deviceResources);
        m_simulateShader->load(L"SimulateParticles_CS.cso");

        m_sortLib = std::make_unique<SortLib>();
        m_sortLib->init(m_deviceResources->GetD3DDevice(), m_deviceResources->GetD3DDeviceContext());
    }

    void RenderParticles::createWindowSizeDependentResources()
    {
        
    }

    void RenderParticles::releaseDeviceDependentResources()
    {
        
    }

    void RenderParticles::update(DX::StepTimer const& timer, Camera* camera /*= nullptr*/)
    {
        if (!camera)
            assert(0);

        m_emitterConstantBufferData.position = DX::toVector4(camera->getPosition() + camera->getForward() * 4.0f);// Vector4(-1.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.direction = Vector4(0.0f, 0.1f, 0.0f, 1.0f);
        m_emitterConstantBufferData.maxSpawn = 1024;

        m_emitFrequence -= timer.GetElapsedSeconds();

        m_particlesGlobalSettingsBufferData.useBillboard = ParticlesGlobals::g_useBillBoard;
    }

    void RenderParticles::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();


        context->UpdateSubresource(m_particlesGlobalSettingsBuffer.Get(), 0, nullptr, &m_particlesGlobalSettingsBufferData, 0, 0);
        //context->PSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());
        //context->VSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());
        context->GSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());
        //context->CSSetConstantBuffers(1, 1, m_particlesGlobalSettingsBuffer.GetAddressOf());

        if (m_resetParticles)
        {
            resetParticles();
            
            m_resetParticles = false;
        }

        if (m_emitFrequence <= 0.0f)
        {
            emitParticles();
            m_emitFrequence = 5.1f;
        }
        simulateParticles();

        m_sortLib->run(m_maxParticles, m_aliveIndexUAV.Get(), m_aliveListCountConstantBuffer.Get());

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
        context->GSSetShader(m_shader->getGeometryShader(), nullptr, 0);
        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

        ID3D11Buffer* nullVertexBuffer = nullptr;
        UINT stride = 0;
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        ID3D11ShaderResourceView* vertexShaderSRVs[] = { m_particleSRV.Get(), m_aliveIndexSRV.Get() };
        context->VSSetShaderResources(0, ARRAYSIZE(vertexShaderSRVs), vertexShaderSRVs);
        context->VSSetConstantBuffers(3, 1, m_aliveListCountConstantBuffer.GetAddressOf());

        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
        context->OMSetDepthStencilState(RenderStatesHelper::DepthRead().Get(), 0);
        context->RSSetState(RenderStatesHelper::CullCounterClockwise().Get());

        context->DrawInstancedIndirect(m_indirectDrawArgsBuffer.Get(), 0);

        ZeroMemory(vertexShaderSRVs, sizeof(vertexShaderSRVs));
        context->VSSetShaderResources(0, ARRAYSIZE(vertexShaderSRVs), vertexShaderSRVs);

        context->GSSetShader(nullptr, nullptr, 0);
    }

    void RenderParticles::setShaderResourceViews(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionView, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalView)
    {
        m_positionView = positionView;
        m_normalView = normalView;
    }

    // Helper function to align values
    int align(int value, int alignment) { return (value + (alignment - 1)) & ~(alignment - 1); }

    void RenderParticles::resetParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        UINT initialCount[] = { 0 };
        m_initDeadListShader->setUAV(0, m_deadListUAV, initialCount);
        initialCount[0] = -1;
        m_initDeadListShader->setUAV(1, m_particleUAV, initialCount);
        m_initDeadListShader->begin();
        m_initDeadListShader->start(align(m_maxParticles, 256) / 256, 1, 1);
        m_initDeadListShader->end();
        m_initDeadListShader->setUAV(0, nullptr);
        m_initDeadListShader->setUAV(1, nullptr);

        //int i = m_initDeadListShader->readCounter(m_deadListUAV);
    }

    void RenderParticles::emitParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        //copy the deadList counter to a constantBuffer
        context->CopyStructureCount(m_deadListCountConstantBuffer.Get(), 0, m_deadListUAV.Get());

        //int i = m_initDeadListShader->readCounter(m_deadListUAV);
        //DebugUtils::log(std::to_string(i));

        //update the number of baked particle (TO DO do it once if no changes)
        context->CopyStructureCount(m_emitterFromBufferConstantBuffer.Get(), 0, m_bakedParticlesUAV.Get());

        UINT initialCount[] = { -1 };
        m_emitFromBufferParticles->setConstantBuffer(1, m_emitterFromBufferConstantBuffer);
        m_emitFromBufferParticles->setConstantBuffer(2, m_deadListCountConstantBuffer);
        m_emitFromBufferParticles->setUAV(0, m_deadListUAV, initialCount);
        m_emitFromBufferParticles->setUAV(1, m_particleUAV, initialCount);
        m_emitFromBufferParticles->setUAV(2, m_bakedParticlesUAV);
        m_emitFromBufferParticles->begin();
        m_emitFromBufferParticles->startIndirect(m_bakedIndirectArgsBuffer);
        m_emitFromBufferParticles->end();
        m_emitFromBufferParticles->setUAV(0, nullptr);
        m_emitFromBufferParticles->setUAV(1, nullptr);
        m_emitFromBufferParticles->setUAV(2, nullptr);

        /*context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);

        UINT initialCount[] = { -1 };
        m_emitParticles->setConstantBuffer(1, m_emitterConstantBuffer);
        m_emitParticles->setConstantBuffer(2, m_deadListCountConstantBuffer);
        m_emitParticles->setUAV(0, m_deadListUAV, initialCount);
        m_emitParticles->setUAV(1, m_particleUAV, initialCount);
        m_emitParticles->begin();
        m_emitParticles->start(align(m_emitterConstantBufferData.maxSpawn, 1024) / 1024, 1, 1);
        m_emitParticles->end();
        m_emitParticles->setUAV(0, nullptr);
        m_emitParticles->setUAV(1, nullptr);*/
    }

    void RenderParticles::simulateParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        UINT initialCount[] = { -1 };
        m_simulateShader->setUAV(0, m_indirectDrawArgsUAV, initialCount);
        m_simulateShader->setUAV(2, m_deadListUAV, initialCount);
        m_simulateShader->setUAV(3, m_particleUAV, initialCount);
        initialCount[0] = 0;
        m_simulateShader->setUAV(1, m_aliveIndexUAV, initialCount);
        m_simulateShader->begin();
        m_simulateShader->start(align(m_maxParticles, 256) / 256, 1, 1);
        m_simulateShader->end();
        m_simulateShader->setUAV(0, nullptr);
        m_simulateShader->setUAV(1, nullptr);
        m_simulateShader->setUAV(2, nullptr);
        m_simulateShader->setUAV(3, nullptr);

        context->CopyStructureCount(m_aliveListCountConstantBuffer.Get(), 0, m_aliveIndexUAV.Get());

        //int i = m_simulateShader->readCounter(m_aliveIndexUAV);
   }

}
