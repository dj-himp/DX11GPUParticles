#include "pch.h"
#include "RenderModelTK.h"

#include "..\Common\DirectXHelper.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    RenderModelTK::RenderModelTK(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : m_deviceResources(deviceResources)
    {

        CreateDeviceDependentResources();
        CreateWindowSizeDependentResources();
    }


    void RenderModelTK::CreateDeviceDependentResources()
    {
        m_states = std::make_unique<DirectX::CommonStates>(m_deviceResources->GetD3DDevice());
        m_fxFactory = std::make_unique<DirectX::EffectFactory>(m_deviceResources->GetD3DDevice());
        m_model = DirectX::Model::CreateFromCMO(m_deviceResources->GetD3DDevice(), L"CatMac.cmo", *m_fxFactory);

        m_world = Matrix::Identity;
        m_view = Matrix::CreateLookAt(Vector3(0.0f, 0.0f, 2.0f), Vector3::Zero, Vector3::UnitY);
    }

    void RenderModelTK::CreateWindowSizeDependentResources()
    {
        m_proj = Matrix::CreatePerspectiveFieldOfView(DirectX::XM_PI / 4.0f, m_deviceResources->GetOutputSize().Width / m_deviceResources->GetOutputSize().Height, 0.1, 100.0f);
    }

    void RenderModelTK::ReleaseDeviceDependentResources()
    {
        m_states.reset();
        m_fxFactory.reset();
        m_model.reset();
    }

    void RenderModelTK::Update(DX::StepTimer const& timer)
    {
    }

    void RenderModelTK::Render()
    {       
        m_model->Draw(m_deviceResources->GetD3DDeviceContext(), *m_states, m_world, m_view, m_proj);
    }

}