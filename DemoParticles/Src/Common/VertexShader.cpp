#include "pch.h"
#include "VertexShader.h"

namespace DemoParticles
{
    VertexShader::VertexShader(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    void VertexShader::load(const std::wstring& vertexFilename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc)
    {
        ID3DBlob* vs_blob;
        DX::ThrowIfFailed(
            D3DReadFileToBlob(vertexFilename.c_str(), &vs_blob)
        );

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateVertexShader(
                vs_blob->GetBufferPointer(),
                vs_blob->GetBufferSize(),
                nullptr,
                &m_vertexShader
            )
        );

        if (vertexDesc.size() > 0)
        {
            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateInputLayout(
                    vertexDesc.data(),
                    (UINT)vertexDesc.size(),
                    vs_blob->GetBufferPointer(),
                    vs_blob->GetBufferSize(),
                    &m_inputLayout
                )
            );
        }
    }

}