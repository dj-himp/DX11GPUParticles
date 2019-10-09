#include "pch.h"
#include "ComputeShader.h"

#include "Common/RenderTarget.h"
#include "Common/DirectXHelper.h"

namespace DemoParticles
{
    ComputeShader::ComputeShader(const DX::DeviceResources* deviceResources, const int nbUAVs /* = 1*/, const bool needCounterBuffer /*= false*/)
        : m_deviceResources(deviceResources)
        , m_nbUAVs(nbUAVs)
        , m_needCounterBuffer(needCounterBuffer)
    {
        m_UAVs.resize(m_nbUAVs);
        m_renderTargets.resize(m_nbUAVs);
    }

    void ComputeShader::load(const std::wstring& computeFilename)
    {
        //const std::vector<byte> vertexShaderData = DX::readBinFile(vertexFilename);

        ID3DBlob* cs_blob;
        D3DReadFileToBlob(computeFilename.c_str(), &cs_blob);

        //ID3DBlob* rootSignature;
        //D3DGetBlobPart(cs_blob, sizeof(cs_blob), D3D_BLOB_ROOT_SIGNATURE, 0, &rootSignature);
        
        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateComputeShader(
                cs_blob->GetBufferPointer(),
                cs_blob->GetBufferSize(),
                nullptr,
                &m_computeShader
            )
        );

        /*ID3D11ShaderReflection* shaderReflector = nullptr;
        D3DReflect(cs_blob->GetBufferPointer(), cs_blob->GetBufferSize(), IID_ID3D11ShaderReflection, (void**)&shaderReflector);
        
        D3D11_SHADER_DESC desc;
        shaderReflector->GetDesc(&desc);

        for (int i = 0; i < desc.BoundResources; ++i)
        {
            //D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
            //shaderReflector->GetInputParameterDesc(i, &paramDesc);

            D3D11_SHADER_INPUT_BIND_DESC paramDesc;
            shaderReflector->GetResourceBindingDesc(i, &paramDesc);
            D3D_SHADER_INPUT_TYPE type = paramDesc.Type;
            D3D_RESOURCE_RETURN_TYPE returnType = paramDesc.ReturnType;
        }*/

        for (int i = 0; i < m_nbUAVs; ++i)
        {
            /*std::unique_ptr<RenderTarget> renderTarget*/m_renderTargets[i] = std::make_unique<RenderTarget>(m_deviceResources, DXGI_FORMAT_R16G16B16A16_FLOAT, m_deviceResources->GetOutputWidth(), m_deviceResources->GetOutputHeight(), 1, true);

            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
            uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
            uavDesc.Texture2D.MipSlice = 0;

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateUnorderedAccessView(/*renderTarget*/m_renderTargets[i]->getTexture().Get(), &uavDesc, &m_UAVs[i])
            );

            //m_renderTargets.push_back(std::move(renderTarget));
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
            
            /*D3D11_SUBRESOURCE_DATA subresourceData;
            subresourceData.pSysMem = &m_counterValue;
            subresourceData.SysMemPitch = 0;
            subresourceData.SysMemSlicePitch = 0;
            */

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateBuffer(&bufferDesc, nullptr/*&subresourceData*/, &m_counterBuffer)
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

        }
    }

    void ComputeShader::begin()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        for (int i = 0; i < m_nbUAVs; ++i)
        {
            context->CSSetUnorderedAccessViews(i, 1, m_UAVs[i].GetAddressOf(), nullptr);
        }
        
        if (m_needCounterBuffer)
        {
            const UINT initialCounts[] = { 0 }; //only use for internal atomic counter
            context->CSSetUnorderedAccessViews(m_nbUAVs, 1, m_counterUAV.GetAddressOf(), initialCounts);
            //context->OMSetRenderTargetsAndUnorderedAccessViews(D3D11_KEEP_RENDER_TARGETS_AND_DEPTH_STENCIL, nullptr, nullptr, m_nbUAVs, 1, m_counterUAV.GetAddressOf(), initialCounts);
        }

        context->CSSetShader(m_computeShader.Get(), nullptr, 0);
    }

    void ComputeShader::end()
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        ID3D11UnorderedAccessView* nullUav[1] = { nullptr };
        for (int i = 0; i < m_nbUAVs + (int)m_needCounterBuffer; ++i)
        {
            
            context->CSSetUnorderedAccessViews(i, 1, nullUav, nullptr);
        }

        context->CSSetShader(nullptr, nullptr, 0);

        //TODO get back the value of the counter
        //CopyStructureCount(m_)
    }

    void ComputeShader::start(int threadGroupX, int threadGroupY, int threadGroupZ)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->Dispatch(threadGroupX, threadGroupY, threadGroupZ);
    }

    void ComputeShader::setShaderResource(int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView)
    {
        auto context = m_deviceResources->GetD3DDeviceContext();

        context->CSSetShaderResources(slot, 1, shaderResourceView.GetAddressOf());
    }

    //TODO maybe do it differently (not returning raw pointer)
    RenderTarget* ComputeShader::getRenderTarget(int slot)
    {
        if (slot >= m_nbUAVs)
        {
            assert(0);
        }

        return m_renderTargets[slot].get();
    }

}