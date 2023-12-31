#pragma once

namespace DX
{
    class DeviceResources;
}

namespace DemoParticles
{
    class VertexShader
    {
    public:
        VertexShader(const DX::DeviceResources* deviceResources);

        void load(const std::wstring& vertexFilename, const std::vector<D3D11_INPUT_ELEMENT_DESC>& vertexDesc);

        ID3D11InputLayout* getInputLayout() { return m_inputLayout.Get(); }
        ID3D11VertexShader* getVertexShader() { return m_vertexShader.Get(); }

    private:
        const DX::DeviceResources* m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11InputLayout>           m_inputLayout;
        Microsoft::WRL::ComPtr<ID3D11VertexShader>          m_vertexShader;
        Microsoft::WRL::ComPtr<ID3D11PixelShader>           m_pixelShader;
        Microsoft::WRL::ComPtr<ID3D11GeometryShader>        m_geometryShader;
    };
}
