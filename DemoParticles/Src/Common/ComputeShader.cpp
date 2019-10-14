#include "pch.h"
#include "ComputeShader.h"

#include "Common/RenderTarget.h"
#include "Common/DirectXHelper.h"

namespace DemoParticles
{
    ComputeShader::ComputeShader(const DX::DeviceResources* deviceResources)//, const int nbUAVs /* = 1*/, const bool needCounterBuffer /*= false*/)
        : m_deviceResources(deviceResources)
        //, m_nbUAVs(nbUAVs)
        //, m_needCounterBuffer(needCounterBuffer)
    {
        //m_UAVs.resize(m_nbUAVs);
        //m_renderTargets.resize(m_nbUAVs);
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

        /*for (int i = 0; i < m_nbUAVs; ++i)
        {
            m_renderTargets[i] = std::make_unique<RenderTarget>(m_deviceResources, DXGI_FORMAT_R16G16B16A16_FLOAT, m_deviceResources->GetOutputWidth(), m_deviceResources->GetOutputHeight(), 1, true);

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = 0;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_renderTargets[i]->getTexture().Get(), &uavDesc, &m_UAVs[i])
            );
        }
      
        if (m_needCounterBuffer)
        {
            D3D11_BUFFER_DESC bufferDesc;
            ZeroMemory(&bufferDesc, sizeof(bufferDesc));
            bufferDesc.ByteWidth = 4;
            bufferDesc.Usage = D3D11_USAGE_DEFAULT;
            bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            bufferDesc.StructureByteStride = 4;
            bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, nullptr, &m_counterBuffer)
            );

            //TODO MAYBE : use Append instead of counter to append at the end of buffer

            D3D11_UNORDERED_ACCESS_VIEW_DESC counterUavDesc;
            ZeroMemory(&counterUavDesc, sizeof(counterUavDesc));
            counterUavDesc.Format = DXGI_FORMAT_UNKNOWN;
            counterUavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            counterUavDesc.Buffer.NumElements = 1;
            counterUavDesc.Buffer.FirstElement = 0;
            counterUavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_COUNTER;
            
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(m_counterBuffer.Get(), &counterUavDesc, &m_counterUAV)
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

        }*/
    }

    void ComputeShader::begin()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        /*for (int i = 0; i < m_nbUAVs; ++i)
        {
            context->CSSetUnorderedAccessViews(i, 1, m_UAVs[i].GetAddressOf(), nullptr);
        }
        
        if (m_needCounterBuffer)
        {
            const UINT initialCounts[] = { 0 }; //only use for internal atomic counter
            context->CSSetUnorderedAccessViews(m_nbUAVs, 1, m_counterUAV.GetAddressOf(), initialCounts);
            //context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, m_nbUAVs, 1, m_counterUAV.GetAddressOf(), initialCounts);
        }*/

        context->CSSetShader(m_computeShader.Get(), nullptr, 0);
    }

    void ComputeShader::end()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        //TODO first copy to a constant buffer to reuse in a shader and copy to cpu only on debug

        /*context->CopyStructureCount(m_counterStagingBuffer.Get(), 0, m_counterUAV.Get());
        D3D11_MAPPED_SUBRESOURCE mappedSubresource;
        context->Map(m_counterStagingBuffer.Get(), 0, D3D11_MAP_READ, 0, &mappedSubresource);
        m_counterValue = *(int*)mappedSubresource.pData;
        context->Unmap(m_counterStagingBuffer.Get(), 0);

        ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
        for (int i = 0; i < m_nbUAVs + (int)m_needCounterBuffer; ++i)
        {
            
            context->CSSetUnorderedAccessViews(i, 1, nullUav, nullptr);
        }
        */
        context->CSSetShader(nullptr, nullptr, 0);
    }

    void ComputeShader::start(int threadGroupX, int threadGroupY, int threadGroupZ)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
    }

    void ComputeShader::setSRV(int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
    }

    void ComputeShader::setUAV(int slot, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        UINT initialCount[] = { 0 };
        context->CSSetUnorderedAccessViews(slot, 1, uav.GetAddressOf(), initialCount);
    }

    void ComputeShader::setConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> buffer)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetConstantBuffers(slot, 1, buffer.GetAddressOf());
    }

    //TODO maybe do it differently (not returning raw pointer)
    /*RenderTarget* ComputeShader::getRenderTarget(int slot)
    {
        if (slot >= m_nbUAVs)
        {
            assert(0);
        }

        return m_renderTargets[slot].get();
    }*/

}