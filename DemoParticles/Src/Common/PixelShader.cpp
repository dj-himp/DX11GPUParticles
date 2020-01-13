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
    }

}