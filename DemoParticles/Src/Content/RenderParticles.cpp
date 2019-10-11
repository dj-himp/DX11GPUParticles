#include "pch.h"
#include "RenderParticles.h"

#include "Common/Shader.h"
#include "Model/Model.h"
#include "Model/MeshFactory.h"
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
    }

    void RenderParticles::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_constantBufferVS.Get(), 0, nullptr, &m_constantBufferDataVS, 0, 0);
        context->UpdateSubresource(m_constantBufferGS.Get(), 0, nullptr, &m_constantBufferDataGS, 0, 0);

        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &m_vertexStride, &offset);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
        context->IASetInputLayout(m_shader->getInputLayout());

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
        context->VSSetConstantBuffers(0, 1, m_constantBufferVS.GetAddressOf());
        context->VSSetSamplers(0, 1, RenderStatesHelper::PointClamp().GetAddressOf());
        context->VSSetShaderResources(0, 1, m_positionView.GetAddressOf());
        context->VSSetShaderResources(1, 1, m_normalView.GetAddressOf());

        context->RSSetState(RenderStatesHelper::CullCounterClockwise().Get());

        context->GSSetShader(m_shader->getGeometryShader(), nullptr, 0);
        context->GSSetConstantBuffers(0, 1, m_constantBufferGS.GetAddressOf());

        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);

        const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Additive().Get(), blend_factor, 0xffffffff);
        context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);
        //context->OMSetDepthStencilState(RenderStatesHelper::DepthDefault().Get(), 0);

        context->Draw(m_nbParticles, 0);

        ID3D11ShaderResourceView* shaderResource[] = { nullptr, nullptr };
        context->VSSetShaderResources(0, 2, shaderResource);
        context->GSSetShaderResources(0, 2, shaderResource);

        context->GSSetShader(nullptr, nullptr, 0);
    }

    void RenderParticles::setShaderResourceViews(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> positionView, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> normalView)
    {
        m_positionView = positionView;
        m_normalView = normalView;
    }

}
