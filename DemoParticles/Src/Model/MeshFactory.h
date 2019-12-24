#pragma once

namespace DemoParticles
{
    class Model;

    class MeshFactory
    {
    public:
        
        static MeshFactory& getInstance();

        void setDeviceResources(const DX::DeviceResources* deviceResources);

        std::unique_ptr<Model> createAxis();
        std::unique_ptr<Model> createQuad();
        std::unique_ptr<Model> createFrustum(const std::vector<DirectX::SimpleMath::Vector3> corners);

        std::vector<D3D11_INPUT_ELEMENT_DESC>& getVertexElements() { return m_vertexElements; }
        UINT getVertexSize() { return m_vertexSize; }

    private:
        MeshFactory();

        const DX::DeviceResources* m_deviceResources;

        std::vector<D3D11_INPUT_ELEMENT_DESC> m_vertexElements;
        UINT m_vertexSize = 0;
    };
}
