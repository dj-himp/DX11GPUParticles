#include "pch.h"
#include "IRenderable.h"

namespace DemoParticles
{
    
    IRenderable::IRenderable(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {
       
    }

    IRenderable::~IRenderable()
    {
        
    }

}