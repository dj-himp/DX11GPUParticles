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
        m_UAVs.reserve(m_nbUAVs);
        m_renderTargets.reserve(m_nbUAVs);
    }

    void ComputeShader::load(const std::wstring& computeFilename)
    {
        //const std::vector<byte> vertexShaderData = DX::readBinFile(vertexFilename);

        ID3DBlob* vs_blob;
        D3DReadFileToBlob(computeFilename.c_str(), &vs_blob);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateComputeShader(
                vs_blob->GetBufferPointer(),
                vs_blob->GetBufferSize(),
                nullptr,
                &m_computeShader
            )
        );

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

        }
    }

    void ComputeShader::begin()
    {

    }

    void ComputeShader::end()
    {

    }

}