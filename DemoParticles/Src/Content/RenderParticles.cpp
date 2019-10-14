#include "pch.h"
#include "RenderParticles.h"

#include "Common/Shader.h"
#include "Common/ComputeShader.h"
#include "Camera/Camera.h"

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
        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc = {
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };
        m_vertexStride = sizeof(Vector2);

        int width = m_deviceResources->GetOutputWidth();
        int height = m_deviceResources->GetOutputHeight();
        m_nbParticles = width * height;
        
        std::vector<Vector2> vertices;
        vertices.resize(m_nbParticles);

        for (int y = 0; y < height; y++)
        {
            float V = (float)y / (float)height;
            for (int x = 0; x < width; x++)
            {
                float U = (float)x / (float)width;
                vertices[width * y + x] = Vector2(U, V);
            }
        }

        D3D11_BUFFER_DESC vertexBufferDesc;
        ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
        vertexBufferDesc.ByteWidth = m_vertexStride * m_nbParticles;
        vertexBufferDesc.StructureByteStride = m_vertexStride;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

        D3D11_SUBRESOURCE_DATA vertexBufferData;
        ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
        vertexBufferData.pSysMem = &vertices[0];
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &m_vertexBuffer)
        );

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderParticles_VS.cso", L"RenderParticles_PS.cso", inputElementDesc, L"RenderParticles_GS.cso");

        CD3D11_BUFFER_DESC constantBufferDescVS(sizeof(m_constantBufferDataVS), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDescVS, nullptr, &m_constantBufferVS)
        );

        CD3D11_BUFFER_DESC constantBufferDescGS(sizeof(m_constantBufferDataGS), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDescGS, nullptr, &m_constantBufferGS)
        );

        
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
        m_initDeadListShader->load(L"InitDeadList_CS.cso");

        m_emitParticles = std::make_unique<ComputeShader>(m_deviceResources);
        m_emitParticles->load(L"EmitParticles_CS.cso");


        //EMITTER CONSTANT BUFFER
        CD3D11_BUFFER_DESC emitterConstantBufferDesc(sizeof(EmitterConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&emitterConstantBufferDesc, nullptr, &m_emitterConstantBuffer)
        );

        //DeadList CONSTANT BUFFER
        CD3D11_BUFFER_DESC deadListCountConstantBufferDesc(sizeof(DeadListCountConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&deadListCountConstantBufferDesc, nullptr, &m_deadListCountConstantBuffer)
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

        m_simulateShader = std::make_unique<ComputeShader>(m_deviceResources);
        m_simulateShader->load(L"SimulateParticles_CS.cso");
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

        m_constantBufferDataVS.world = m_world.Transpose();
        
        m_constantBufferDataGS.world = m_world.Transpose();
        m_constantBufferDataGS.view = camera->getView().Transpose();
        m_constantBufferDataGS.projection = camera->getProjection().Transpose();

        m_emitterConstantBufferData.position = Vector4(0.0f, 0.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.direction = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
        m_emitterConstantBufferData.maxSpawn = 1024;

    }

    void RenderParticles::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        if (m_resetParticles)
        {
            resetParticles();
            
            m_resetParticles = false;
        }

        emitParticles();
        simulateParticles();

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

        m_initDeadListShader->setUAV(0, m_deadListUAV);
        m_initDeadListShader->setUAV(1, m_particleUAV);
        m_initDeadListShader->begin();
        m_initDeadListShader->start(align(m_maxParticles, 256) / 256, 1, 1);
        m_initDeadListShader->end();
        m_initDeadListShader->setUAV(0, nullptr);
        m_initDeadListShader->setUAV(1, nullptr);

        m_deadListCountConstantBufferData.nbDeadParticles = m_maxParticles;
    }

    void RenderParticles::emitParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_emitterConstantBuffer.Get(), 0, nullptr, &m_emitterConstantBufferData, 0, 0);
        context->UpdateSubresource(m_deadListCountConstantBuffer.Get(), 0, nullptr, &m_deadListCountConstantBufferData, 0, 0);

        m_emitParticles->setConstantBuffer(0, m_emitterConstantBuffer);
        m_emitParticles->setConstantBuffer(1, m_deadListCountConstantBuffer);
        m_emitParticles->setUAV(0, m_deadListUAV);
        m_emitParticles->setUAV(1, m_particleUAV);
        m_emitParticles->begin();
        m_emitParticles->start(align(m_maxParticles, 256) / 256, 1, 1);
        m_emitParticles->end();
        m_emitParticles->setUAV(0, nullptr);
        m_emitParticles->setUAV(1, nullptr);
    }

    void RenderParticles::simulateParticles()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        m_simulateShader->setUAV(0, m_indirectDrawArgsUAV);
        m_simulateShader->begin();
        m_simulateShader->start(align(m_maxParticles, 256) / 256, 1, 1);
        m_simulateShader->end();
        m_simulateShader->setUAV(0, nullptr);
   }

}
