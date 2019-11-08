#include "pch.h"
#include "IParticleEmitter.h"

namespace DemoParticles
{

    IParticleEmitter::IParticleEmitter(const DX::DeviceResources* deviceResources)
        : m_deviceResources(deviceResources)
    {

    }

    void IParticleEmitter::update(DX::StepTimer const& timer)
    {
        //false to reset if the previous render emit particles
        m_needEmit = false;

        m_lastEmitTime -= timer.GetElapsedSeconds();
        if (m_lastEmitTime <= 0.0)
        {
            m_lastEmitTime = m_emitDelay;
            m_needEmit = true;
        }
    }

}