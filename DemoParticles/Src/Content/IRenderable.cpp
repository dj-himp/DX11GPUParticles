#include "pch.h"
#include "IRenderable.h"

namespace DemoParticles
{
    
    IRenderable::IRenderable(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : m_deviceResources(deviceResources)
    {
       
    }

    IRenderable::~IRenderable()
    {
        
    }

    void IRenderable::init()
    {
        createDeviceDependentResources();
        createWindowSizeDependentResources();

    }

    void IRenderable::release()
    {
        releaseDeviceDependentResources();
    }
}