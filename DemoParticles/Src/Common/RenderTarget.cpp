#include "pch.h"
#include "RenderTarget.h"

namespace DemoParticles
{
    RenderTarget::RenderTarget(const DX::DeviceResources* deviceResources, DXGI_FORMAT format, int width, int height, int mipLevel /*= 1*/, bool isUnordered /*= false*/)
    {
        D3D11_TEXTURE2D_DESC textureDesc;
        textureDesc.ArraySize = 1;
        textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
        if (isUnordered)
        {
            textureDesc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
        }
        textureDesc.CPUAccessFlags = 0;
        textureDesc.Format = format;
        textureDesc.Width = width;
        textureDesc.Height = height;
        textureDesc.MipLevels = mipLevel;
        if (mipLevel > 1)
        {
            textureDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
        }
        else
        {
            textureDesc.MiscFlags = 0;
        }

        textureDesc.SampleDesc = { 1, 0 };
        textureDesc.Usage = D3D11_USAGE_DEFAULT;

        DX::ThrowIfFailed(
            deviceResources->GetD3DDevice()->CreateTexture2D(&textureDesc, nullptr, &m_texture)
        );

        D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc;
        shaderResourceDesc.Format = format;
        shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        shaderResourceDesc.Texture2D.MostDetailedMip = 0;
        shaderResourceDesc.Texture2D.MipLevels = mipLevel;

        DX::ThrowIfFailed(
            deviceResources->GetD3DDevice()->CreateShaderResourceView(m_texture.Get(), &shaderResourceDesc, &m_shaderResourceView)
        );

        D3D11_RENDER_TARGET_VIEW_DESC renderTargetView;
        renderTargetView.Format = format;
        renderTargetView.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
        renderTargetView.Texture2D.MipSlice = 0;

        DX::ThrowIfFailed(
            deviceResources->GetD3DDevice()->CreateRenderTargetView(m_texture.Get(), &renderTargetView, &m_renderTargetView)
        );

    }

}