#include "pch.h"
#include "IRenderable.h"

//needed to have std::unique_ptr<Shader> in forward declaration : 
//it needs to know the object shader to find the destructor
#include "Common/Shader.h"

namespace DemoParticles
{
    
    IRenderable::IRenderable(const DX::DeviceResources* deviceResources)
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