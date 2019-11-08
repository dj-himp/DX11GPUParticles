#pragma once

namespace DemoParticles
{
    class IParticleEmitter
    {
    public:
        IParticleEmitter(const DX::DeviceResources* deviceResources);

        virtual void createDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer);
        virtual void emit() = 0;

    protected:
        const DX::DeviceResources* m_deviceResources;

        float m_emitDelay = 0.0f;
        float m_lastEmitTime = 0.0f;
        bool m_needEmit = false;
    };
}
