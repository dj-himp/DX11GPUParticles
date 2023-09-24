#include "pch.h"
#include "PixelShader.h"

namespace DemoParticles
{
    PixelShader::PixelShader(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    void PixelShader::load(const std::wstring& pixelFilename)
    {
        ID3DBlob* ps_blob;
        D3DReadFileToBlob(pixelFilename.c_str(), &ps_blob);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreatePixelShader(
                ps_blob->GetBufferPointer(),
                ps_blob->GetBufferSize(),
                nullptr,
                &m_pixelShader
            )
        );

        D3D11_BUFFER_DESC stagingDesc;
        ZeroMemory(&stagingDesc, sizeof(stagingDesc));
        stagingDesc.Usage = D3D11_USAGE_STAGING;
        stagingDesc.ByteWidth = sizeof(UINT);
        stagingDesc.BindFlags = 0;
        stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(&stagingDesc, nullptr, &m_counterStagingBuffer)
        );
    }

    void PixelShader::setUAVs(int slot, int count, ID3D11UnorderedAccessView* uavs[], UINT initialCount[] /*= nullptr*/)
    {
        //WARNING
        //UAV start as 1 because render targets takes first slots (here only 1 RT)

        auto context = m_deviceResources->GetD3DDeviceContext();

        context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, slot, count, uavs, initialCount);
    }

    int PixelShader::readCounter(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();
        int count = 0;

        // Copy the UAV counter to a staging resource
        context->CopyStructureCount(m_counterStagingBuffer.Get(), 0, uav.Get());

        // Map the staging resource
        D3D11_MAPPED_SUBRESOURCE MappedResource;
        context->Map(m_counterStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &MappedResource);

        // Read the data
        count = *(int*)MappedResource.pData;

        context->Unmap(m_counterStagingBuffer.Get(), 0);

        return count;
    }
}