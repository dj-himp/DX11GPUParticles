#pragma once

namespace DemoParticles
{
    class IParticleEmitter
    {
    public:
        IParticleEmitter(const DX::DeviceResources* deviceResources);

        virtual void createDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer) = 0;
        virtual void emit() = 0;

    protected:
        const DX::DeviceResources* m_deviceResources;

    };
}
