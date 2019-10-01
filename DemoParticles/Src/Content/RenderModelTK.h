#pragma once

#include "ShaderStructures.h"

namespace DemoParticles
{
    class RenderModelTK
    {
    public:
        RenderModelTK(const std::shared_ptr<DX::DeviceResources>& deviceResources);

        void CreateDeviceDependentResources();
        void CreateWindowSizeDependentResources();
        void ReleaseDeviceDependentResources();

        void Update(DX::StepTimer const& timer);
        void Render();

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;

        std::unique_ptr<DirectX::CommonStates> m_states;
        std::unique_ptr<DirectX::IEffectFactory> m_fxFactory;
        std::unique_ptr<DirectX::Model> m_model;

        DirectX::SimpleMath::Matrix m_world;
        DirectX::SimpleMath::Matrix m_view;
        DirectX::SimpleMath::Matrix m_proj;
    };
}
