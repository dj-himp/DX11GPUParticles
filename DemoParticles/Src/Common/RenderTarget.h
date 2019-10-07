#pragma once

namespace DX
{
    class DeviceResources;
}

namespace DemoParticles
{
    class RenderTarget
    {
    public:
        RenderTarget(const DX::DeviceResources* deviceResources, DXGI_FORMAT format, int width, int height, int mipLevel = 1, bool isUnordered = false);

        Microsoft::WRL::ComPtr <ID3D11Texture2D> getTexture() { return m_texture; }
        Microsoft::WRL::ComPtr <ID3D11RenderTargetView> getRenderTargetView() { return m_renderTargetView; }
        Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> getShaderResourceView() { return m_shaderResourceView; }

    private:

        Microsoft::WRL::ComPtr <ID3D11Texture2D> m_texture;
        Microsoft::WRL::ComPtr <ID3D11RenderTargetView> m_renderTargetView;
        Microsoft::WRL::ComPtr <ID3D11ShaderResourceView> m_shaderResourceView;

    };
}
