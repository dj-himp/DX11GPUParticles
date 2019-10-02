#pragma once

namespace DemoParticles
{
    class Model;

    class AxisCreator
    {
    public:
        AxisCreator(const DX::DeviceResources* deviceResources);
        ~AxisCreator();

        std::unique_ptr<Model> create();

    private:
        const const DX::DeviceResources* m_deviceResources;
    };
}
