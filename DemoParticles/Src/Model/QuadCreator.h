#pragma once

namespace DemoParticles
{
    class Model;

    class QuadCreator
    {
    public:
        QuadCreator(const std::shared_ptr<DX::DeviceResources>& deviceResources);
        ~QuadCreator();

        std::unique_ptr<Model> create();

    private:
        std::shared_ptr<DX::DeviceResources> m_deviceResources;
    };
}