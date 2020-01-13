#pragma once

namespace DX
{
    class DeviceResources;
}

namespace DemoParticles
{
    class GeometryShader
    {
    public:
        GeometryShader(const DX::DeviceResources* deviceResources);

        void load(const std::wstring& geometryShader);

        ID3D11GeometryShader* getGeometryShader() { return m_geometryShader.Get(); }

    private:
        const DX::DeviceResources* m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11GeometryShader>        m_geometryShader;
    };
}
