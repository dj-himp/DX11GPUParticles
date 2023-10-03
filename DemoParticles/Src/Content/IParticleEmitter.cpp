#include "pch.h"
#include "IParticleEmitter.h"

namespace DemoParticles
{

    IParticleEmitter::IParticleEmitter(const DX::DeviceResources* deviceResources, std::string name, EmitterType type)
        : m_deviceResources(deviceResources)
        , m_name(name)
        , m_type(type)
    {

    }

    IParticleEmitter::~IParticleEmitter()
    {

    }
}