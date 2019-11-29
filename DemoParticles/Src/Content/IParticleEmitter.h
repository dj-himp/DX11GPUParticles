#pragma once

using json = nlohmann::json;

namespace DemoParticles
{
    class Camera;

    class IParticleEmitter
    {
    public:
        IParticleEmitter(const DX::DeviceResources* deviceResources);

        virtual void createDeviceDependentResources() = 0;

        virtual void update(DX::StepTimer const& timer) = 0;
        virtual void emit() = 0;
        virtual void RenderImGui(Camera* camera) {}
        virtual void reset() {}
        virtual void save(json& file) {}
        virtual void load(json& file) {}

    protected:
        const DX::DeviceResources* m_deviceResources;

        bool m_enabled = false;
    };
}
