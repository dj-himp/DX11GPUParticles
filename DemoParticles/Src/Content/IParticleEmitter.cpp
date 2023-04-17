#include "pch.h"
#include "IParticleEmitter.h"

namespace DemoParticles
{

    IParticleEmitter::IParticleEmitter(const DX::DeviceResources* deviceResources, std::string name)
        : m_deviceResources(deviceResources)
        , m_name(name)
    {

    }
}