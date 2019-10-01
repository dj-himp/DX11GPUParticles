#pragma once

namespace DemoParticles
{
    class Model;

    class AxisCreator
    {
    public:
        AxisCreator(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~AxisCreator();

        std::unique_ptr<Model> create();

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
    };
}
