#pragma once

namespace DemoParticles
{
    class RenderStatesHelper
    {
    public:

        static RenderStatesHelper& instance();

        void init(const DX::DeviceResources* deviceResources);

        static Microsoft::WRL::ComPtr<ID3D11BlendState> Opaque() { return instance().m_opaque; }
        static Microsoft::WRL::ComPtr<ID3D11BlendState> AlphaBlend() { return instance().m_alphaBlend; }
        static Microsoft::WRL::ComPtr<ID3D11BlendState> Additive() { return instance().m_additive; }

        static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthNone() { return instance().m_depthNone; }
        static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthDefault() { return instance().m_depthDefault; }
        static Microsoft::WRL::ComPtr<ID3D11DepthStencilState> DepthRead() { return instance().m_depthRead; }

        static Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullNone() { return instance().m_cullNone; }
        static Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullClockwise() { return instance().m_cullClockwise; }
        static Microsoft::WRL::ComPtr<ID3D11RasterizerState> CullCounterClockwise() { return instance().m_cullCounterClockwise; }
        static Microsoft::WRL::ComPtr<ID3D11RasterizerState> Wireframe() { return instance().m_wireframe; }

        static Microsoft::WRL::ComPtr<ID3D11SamplerState> PointWrap() { return instance().m_pointWrap; }
        static Microsoft::WRL::ComPtr<ID3D11SamplerState> PointClamp() { return instance().m_pointClamp; }
        static Microsoft::WRL::ComPtr<ID3D11SamplerState> LinearWrap() { return instance().m_linearWrap; }
        static Microsoft::WRL::ComPtr<ID3D11SamplerState> LinearClamp() { return instance().m_linearClamp; }
        static Microsoft::WRL::ComPtr<ID3D11SamplerState> AnisotropicWrap() { return instance().m_anisotropicWrap; }
        static Microsoft::WRL::ComPtr<ID3D11SamplerState> AnisotropicClamp() { return instance().m_anisotropicClamp; }
        
    private:

        RenderStatesHelper();

        //const DX::DeviceResources* m_deviceResources;

        Microsoft::WRL::ComPtr<ID3D11BlendState> m_opaque;
        Microsoft::WRL::ComPtr<ID3D11BlendState> m_alphaBlend;
        Microsoft::WRL::ComPtr<ID3D11BlendState> m_additive;

        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthNone;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthDefault;
        Microsoft::WRL::ComPtr<ID3D11DepthStencilState> m_depthRead;

        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_cullNone;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_cullClockwise;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_cullCounterClockwise;
        Microsoft::WRL::ComPtr<ID3D11RasterizerState> m_wireframe;

        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pointWrap;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_pointClamp;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_linearWrap;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_linearClamp;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_anisotropicWrap;
        Microsoft::WRL::ComPtr<ID3D11SamplerState> m_anisotropicClamp;
    };
}
