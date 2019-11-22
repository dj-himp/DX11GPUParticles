#pragma once
#include "IRenderable.h"

#include "ShaderStructures.h"
#include "Common/FGAParser.h"

namespace DemoParticles
{
    class Shader;

    class RenderForceField : public IRenderable
    {
    public:
        RenderForceField(const DX::DeviceResources* deviceResources);

        virtual void init() override;
        virtual void createDeviceDependentResources() override;
        virtual void createWindowSizeDependentResources() override;
        virtual void releaseDeviceDependentResources() override;

        virtual void update(DX::StepTimer const& timer, Camera* camera = nullptr) override;
        virtual void render() override;

    private:
        Microsoft::WRL::ComPtr<ID3D11Texture3D>             m_forceFieldTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>    m_forceFieldTextureSRV;
        FGAParser::FGAContent                               m_content;

        std::unique_ptr<Shader>                             m_shader;

        RenderForceFieldConstantBuffer                      m_constantBufferData;
        Microsoft::WRL::ComPtr<ID3D11Buffer>                m_constantBuffer;
    };
}
