#include "pch.h"
#include "GeometryShader.h"

namespace DemoParticles
{
    GeometryShader::GeometryShader(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    void GeometryShader::load(const std::wstring& geometryShader)
    {
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