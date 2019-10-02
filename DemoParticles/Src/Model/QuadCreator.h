#pragma once

namespace DemoParticles
{
    class Model;

    class QuadCreator
    {
    public:
        QuadCreator(const DX::DeviceResources* deviceResources);
        ~QuadCreator();

        std::unique_ptr<Model> create();

    private:
        const const DX::DeviceResources* m_deviceResources;
    };
}