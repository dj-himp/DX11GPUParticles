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
        ComputeShader(const DX::DeviceResources* deviceResources, const int nbUAVs = 1, const bool needCounterBuffer = false);

        void load(const std::wstring& computeFilename);
        void begin();
        void end();
        void start(int threadGroupX, int threadGroupY, int threadGroupZ);

        void setShaderResource(int slot, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView);

        ID3D11ComputeShader* getVertexShader() { return m_computeShader.Get(); }

        RenderTarget* getRenderTarget(int slot);

    private:
        const const DX::DeviceResources* m_deviceResources;

        bool m_needCounterBuffer;
        int m_nbUAVs;
        std::vector<Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>>  m_UAVs;
        std::vector<std::unique_ptr<RenderTarget>>                      m_renderTargets;
        
        Microsoft::WRL::ComPtr<ID3D11Buffer>                            m_counterBuffer;
        int                                                             m_counterValue = 0;
        Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView>               m_counterUAV;

        Microsoft::WRL::ComPtr<ID3D11ComputeShader>          m_computeShader;
    };
}
