#include "pch.h"
#include "RenderStatesHelper.h"


namespace DemoParticles
{

    RenderStatesHelper& RenderStatesHelper::instance()
    {
        static RenderStatesHelper instance = RenderStatesHelper();
        return instance;
    }

    void RenderStatesHelper::init(const DX::DeviceResources* deviceResources)
    {
        //Blend Opaque
        {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateBlendState(&desc, &m_opaque)
            );
        }

        //Blend AlphaBlend
        {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateBlendState(&desc, &m_alphaBlend)
            );
        }

        //Blend Additive
        {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateBlendState(&desc, &m_additive)
            );
        }

        //Blend Non Premultiplied
        {
            D3D11_BLEND_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.AlphaToCoverageEnable = FALSE;
            desc.IndependentBlendEnable = FALSE;
            desc.RenderTarget[0].BlendEnable = TRUE;
            desc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
            desc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
            desc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
            desc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateBlendState(&desc, &m_nonPremultiplied)
            );
        }


        //Depth None
        {
            D3D11_DEPTH_STENCIL_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.DepthEnable = FALSE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

            desc.StencilEnable = FALSE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

            desc.BackFace = desc.FrontFace;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateDepthStencilState(&desc, &m_depthNone)
            );
        }

        //Depth Default
        {
            D3D11_DEPTH_STENCIL_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

            desc.StencilEnable = FALSE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

            desc.BackFace = desc.FrontFace;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateDepthStencilState(&desc, &m_depthDefault)
            );
        }

        //Depth Read
        {
            D3D11_DEPTH_STENCIL_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.DepthEnable = TRUE;
            desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
            desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

            desc.StencilEnable = FALSE;
            desc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
            desc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;

            desc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
            desc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
            desc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;

            desc.BackFace = desc.FrontFace;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateDepthStencilState(&desc, &m_depthRead)
            );
        }

        //Rasterizer CullNone
        {
            D3D11_RASTERIZER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.CullMode = D3D11_CULL_NONE;
            desc.FillMode = D3D11_FILL_SOLID;
            desc.DepthClipEnable = TRUE;
            desc.MultisampleEnable = TRUE;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateRasterizerState(&desc, &m_cullNone)
            );
        }

        //Rasterizer CullClockwise
        {
            D3D11_RASTERIZER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.CullMode = D3D11_CULL_FRONT;
            desc.FillMode = D3D11_FILL_SOLID;
            desc.DepthClipEnable = TRUE;
            desc.MultisampleEnable = TRUE;
            desc.FrontCounterClockwise = TRUE; //TRUE because my engine is code with right handed coordinates

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateRasterizerState(&desc, &m_cullClockwise)
            );
        }

        //Rasterizer CullCounterClockwise
        {
            D3D11_RASTERIZER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.CullMode = D3D11_CULL_BACK;
            desc.FillMode = D3D11_FILL_SOLID;
            desc.DepthClipEnable = TRUE;
            desc.MultisampleEnable = TRUE;
            desc.FrontCounterClockwise = TRUE; //TRUE because my engine is code with right handed coordinates

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateRasterizerState(&desc, &m_cullCounterClockwise)
            );
        }

        //Rasterizer Wireframe
        {
            D3D11_RASTERIZER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.CullMode = D3D11_CULL_NONE;
            desc.FillMode = D3D11_FILL_WIREFRAME;
            desc.DepthClipEnable = TRUE;
            desc.MultisampleEnable = TRUE;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateRasterizerState(&desc, &m_wireframe)
            );
        }

        //Sampler PointWrap
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_pointWrap)
            );
        }

        //Sampler PointClamp
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_pointClamp)
            );
        }

        //Sampler PointBorder
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            //no need to initialized because of zeroMemory
            //desc.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f };

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_pointBorder)
            );
        }

        //Sampler LinearWrap
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_linearWrap)
            );
        }

        //Sampler LinearClamp
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_linearClamp)
            );
        }

        //Sampler LinearBorder
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            //no need to initialized because of zeroMemory
            //desc.BorderColor = { 0.0f, 0.0f, 0.0f, 0.0f };

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_linearBorder)
            );
        }

        //Sampler AnisotropicWrap
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_ANISOTROPIC;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_anisotropicWrap)
            );
        }

        //Sampler AnisotropicClamp
        {
            D3D11_SAMPLER_DESC desc;
            ZeroMemory(&desc, sizeof(desc));
            desc.Filter = D3D11_FILTER_ANISOTROPIC;

            desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
            desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;

            desc.MaxAnisotropy = deviceResources->GetDeviceFeatureLevel() > D3D_FEATURE_LEVEL_9_1 ? D3D11_MAX_MAXANISOTROPY : 2u;

            desc.MaxLOD = FLT_MAX;
            desc.ComparisonFunc = D3D11_COMPARISON_NEVER;

            DX::ThrowIfFailed(
                deviceResources->GetD3DDevice()->CreateSamplerState(&desc, &m_anisotropicClamp)
            );
        }
    }

    RenderStatesHelper::RenderStatesHelper()
    {
        
    }
}