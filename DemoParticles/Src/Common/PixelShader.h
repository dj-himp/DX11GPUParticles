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
        void setUAVs(int slot, int count, ID3D11UnorderedAccessView* uavs[], UINT initialCount[] = nullptr);
        void setSRV(int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);

        ID3D11PixelShader* getPixelShader() { return m_pixelShader.Get(); }

        int readCounter(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav);

    private:
        const DX::DeviceResources* m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                            m_counterStagingBuffer;

        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
    };
}
