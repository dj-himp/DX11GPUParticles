#pragma once

namespace DX
{
    class DeviceResources;
}

namespace DemoParticles
{
    class PixelShader
    {
    public:
        PixelShader(const DX::DeviceResources* deviceResources);

        void load(const std::wstring& pixelFilename);

        ID3D11PixelShader* getPixelShader() { return m_pixelShader.Get(); }

    private:
        const DX::DeviceResources* m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
    };
}
