#include "pch.h"
#include "ComputeShader.h"

#include "Common/RenderTarget.h"

namespace DemoParticles
{
    ComputeShader::ComputeShader(const DX::DeviceResources* deviceResources)//, const int nbUAVs /* = 1*/, const bool needCounterBuffer /*= false*/)
        : m_deviceResources(deviceResources)
    {
        
    }

    void ComputeShader::load(const std::wstring& computeFilename)
    {
        ID3DBlob* cs_blob;
        D3DReadFileToBlob(computeFilename.c_str(), &cs_blob);
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateComputeShader(
                cs_blob->GetBufferPointer(),
                cs_blob->GetBufferSize(),
                nullptr,
                &m_computeShader
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

    void ComputeShader::begin()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetShader(m_computeShader.Get(), nullptr, 0);
    }

    void ComputeShader::end()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetShader(nullptr, nullptr, 0);
    }

    void ComputeShader::start(int threadGroupX, int threadGroupY, int threadGroupZ)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
    }

    void ComputeShader::startIndirect(Microsoft::WRL::ComPtr<ID3D11Buffer> indirectBuffer, UINT alignedByteOffsetForArgs /*= 0*/)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->DispatchIndirect(indirectBuffer.Get(), alignedByteOffsetForArgs);
    }

    void ComputeShader::setSRV(int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
    }

    void ComputeShader::setUAV(int slot, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav, UINT initialCount[] /*= nullptr*/)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        //UINT initialCount[] = { 0 };
        context->CSSetUnorderedAccessViews(slot, 1, uav.GetAddressOf(), initialCount);
    }

    void ComputeShader::setConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> buffer)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
    }

    int ComputeShader::readCounter(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav)
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