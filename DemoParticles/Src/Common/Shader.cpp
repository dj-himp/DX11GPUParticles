#include "pch.h"
#include "Shader.h"

#include "Common/DirectXHelper.h"

namespace DemoParticles
{
    Shader::Shader(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    void Shader::load(const std::wstring& vertexFilename, const std::wstring& pixelFilename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc, const std::wstring& geometryShader /*= L""*/)
    {
        //const std::vector<byte> vertexShaderData = DX::readBinFile(vertexFilename);

        ID3DBlob* vs_blob;
        D3DReadFileToBlob(/*L"../x64/Debug/RenderDebugColor_VS.cso"*/vertexFilename.c_str(), &vs_blob);

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateVertexShader(
                vs_blob->GetBufferPointer(),
                vs_blob->GetBufferSize(),
                nullptr,
                &m_vertexShader
            )
        );

        /*DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateVertexShader(
                &vertexShaderData[0],
                vertexShaderData.size(),
                nullptr,
                &m_vertexShader
            )
        );*/

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateInputLayout(
                vertexDesc.data(),
                vertexDesc.size(),
                vs_blob->GetBufferPointer(),
                vs_blob->GetBufferSize(),
                &m_inputLayout
            )
        );

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

        if (geometryShader.empty() == false)
        {
            ID3DBlob* gs_blob;
            D3DReadFileToBlob(geometryShader.c_str(), &gs_blob);

            DX::ThrowIfFailed(
                m_deviceResources->GetD3DDevice()->CreateGeometryShader(
                    gs_blob->GetBufferPointer(),
                    gs_blob->GetBufferSize(),
                    nullptr,
                    &m_geometryShader)
            );
        }
    }

}