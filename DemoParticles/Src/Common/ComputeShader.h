#pragma once

namespace DX
{
    class DeviceResources;
}

namespace DemoParticles
{
    class RenderTarget;

    class ComputeShader
    {
    public:
        ComputeShader(const DX::DeviceResources* deviceResources);// , const int nbUAVs = 1, const bool needCounterBuffer = false);

        void load(const std::wstring& computeFilename);
        void begin();
        void end();
        void start(int threadGroupX, int threadGroupY, int threadGroupZ);
        void startIndirect(Microsoft::WRL::ComPtr<ID3D11Buffer> inderectBuffer, UINT alignedByteOffsetForArgs = 0);

        void setSRV(int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);
        void setUAV(int slot, Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav, UINT initialCount[] = nullptr);
        void setConstantBuffer(int slot, Microsoft::WRL::ComPtr<ID3D11Buffer> buffer);

        ID3D11ComputeShader* getShader() { return m_computeShader.Get(); }

        RenderTarget* getRenderTarget(int slot);

        int readCounter(Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> uav);

    private:
        const DX::DeviceResources* m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11Buffer>                            m_counterStagingBuffer;

        Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_computeShader;
    };
}
