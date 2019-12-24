#include "pch.h"
#include "RenderForceField.h"

#include "Common/Shader.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderForceField::RenderForceField(const DX::DeviceResources* deviceResources)
        : IRenderable(deviceResources)
    {
        init();
    }

    void RenderForceField::init()
    {
        IRenderable::init();
    }

    void RenderForceField::createDeviceDependentResources()
    {
        FGAParser parser;
        //parser.parse("forceFieldTest.fga", m_content);
        parser.parse("VF_Vortex.fga", m_content);
        //parser.parse("VF_Smoke.fga", m_content);
        //parser.parse("VF_Turbulence.fga", m_content);
        //parser.parse("VF_FluidVol.fga", m_content);
        //parser.parse("VF_Point.fga", m_content);

        D3D11_TEXTURE3D_DESC desc;
        desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
        desc.CPUAccessFlags = 0;
        desc.Width = m_content.sizeX;
        desc.Height = m_content.sizeY;
        desc.Depth = m_content.sizeZ;
        desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        desc.MipLevels = 1;
        desc.Usage = D3D11_USAGE_DEFAULT;
        desc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA data;
        data.pSysMem = &m_content.forces[0];
        data.SysMemPitch = m_content.sizeX;
        data.SysMemSlicePitch = m_content.sizeY;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateTexture3D(&desc, &data, &m_forceFieldTexture)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC forceFieldTextureSRVDesc;
        forceFieldTextureSRVDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        forceFieldTextureSRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE3D;
        forceFieldTextureSRVDesc.Texture3D.MipLevels = 1;
        forceFieldTextureSRVDesc.Texture3D.MostDetailedMip = 0;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateShaderResourceView(m_forceFieldTexture.Get(), &forceFieldTextureSRVDesc, &m_forceFieldTextureSRV)
        );


        CD3D11_BUFFER_DESC constantBufferDesc(sizeof(RenderForceFieldConstantBuffer), D3D11_BIND_CONSTANT_BUFFER);
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, nullptr, &m_constantBuffer)
        );

        //NEED REFACTOR of Shader::Load to remove this 
        std::vector<D3D11_INPUT_ELEMENT_DESC> inputElementDesc;/* = {
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };*/

        m_shader = std::make_unique<Shader>(m_deviceResources);
        m_shader->load(L"RenderForceField_VS.cso", L"RenderForceField_PS.cso", inputElementDesc, L"RenderForceField_GS.cso");

        
    }

    void RenderForceField::createWindowSizeDependentResources()
    {
        
    }

    void RenderForceField::releaseDeviceDependentResources()
    {
        
    }

    void RenderForceField::update(DX::StepTimer const& /*timer*/, Camera* /*camera*/ /*= nullptr*/)
    {
        m_constantBufferData.size = Vector3((float)m_content.sizeX, (float)m_content.sizeY, (float)m_content.sizeZ);
    }

    void RenderForceField::render()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->UpdateSubresource(m_constantBuffer.Get(), 0, nullptr, &m_constantBufferData, 0, 0);

        context->VSSetShader(m_shader->getVertexShader(), nullptr, 0);
        context->GSSetShader(m_shader->getGeometryShader(), nullptr, 0);
        context->PSSetShader(m_shader->getPixelShader(), nullptr, 0);
        
        ID3D11Buffer* nullVertexBuffer = nullptr;
        UINT stride = 0;
        UINT offset = 0;
        context->IASetVertexBuffers(0, 1, &nullVertexBuffer, &stride, &offset);
        context->IASetIndexBuffer(nullptr, DXGI_FORMAT_UNKNOWN, 0);
        context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

        context->VSSetConstantBuffers(4, 1, m_constantBuffer.GetAddressOf());
        ID3D11ShaderResourceView* SRVs[] = { m_forceFieldTextureSRV.Get() };
        context->VSSetShaderResources(0, ARRAYSIZE(SRVs), SRVs);
        context->VSSetSamplers(0, 1, RenderStatesHelper::PointClamp().GetAddressOf());

        const float blendFactor[4] = { 0.f, 0.f, 0.f, 0.f };
        context->OMSetBlendState(RenderStatesHelper::Opaque().Get(), blendFactor, 0xffffffff);
        context->RSSetState(RenderStatesHelper::CullNone().Get());
        context->OMSetDepthStencilState(RenderStatesHelper::DepthNone().Get(), 0);

        context->DrawInstanced((UINT)m_content.forces.size(), 1, 0, 0);

        //ZeroMemory(SRVs, ARRAYSIZE(SRVs));
        SRVs[0] = nullptr;
        context->VSSetShaderResources(0, ARRAYSIZE(SRVs), SRVs);

        context->GSSetShader(nullptr, nullptr, 0);
    }
}