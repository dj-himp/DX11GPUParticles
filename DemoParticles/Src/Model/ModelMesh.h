#pragma once

namespace DemoParticles
{
    class ModelMesh
    {
    public:

        ModelMesh(const DX::DeviceResources* deviceResources);
        //~ModelMesh();

        void AddTextureDiffuse(const std::string& path);

        void setInputElements(std::vector<D3D11_INPUT_ELEMENT_DESC> inputElements) { m_inputElements = inputElements; }
        void setVertexSize(int vertexSize) { m_vertexSize = vertexSize; }
        void setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY primitiveTopology) { m_primitiveTopology = primitiveTopology; }
        D3D11_PRIMITIVE_TOPOLOGY getPrimitiveTopology() { return m_primitiveTopology; }

        void setVertexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer) { m_vertexBuffer = vertexBuffer; }
        const Microsoft::WRL::ComPtr<ID3D11Buffer> getVertexBuffer() { return m_vertexBuffer; }

        void setIndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer) { m_indexBuffer = indexBuffer; }
        const Microsoft::WRL::ComPtr<ID3D11Buffer> getIndexBuffer() { return m_indexBuffer; }

        void setVertexSRV(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> vertexSRV) { m_vertexSRV = vertexSRV; }
        const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getVertexSRV() { return m_vertexSRV; }
        void setindexSRV(Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> indexSRV) { m_indexSRV = indexSRV; }
        const Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getIndexSRV() { return m_indexSRV; }

        void setVertexCount(int vertexCount) { m_vertexCount = vertexCount; }
        int getVertexCount() { return m_vertexCount; }

        void setPrimitiveCount(int primitiveCount) { m_primitiveCount = primitiveCount; }
        void setIndexCount(int indexCount) { m_indexCount = indexCount; }
        int getIndexCount() { return m_indexCount; }

        int getBaseVertex() const { return m_baseVertex; }
        void setBaseVertex(int val) { m_baseVertex = val; }

    private:
        const DX::DeviceResources* m_deviceResources;

        std::vector<D3D11_INPUT_ELEMENT_DESC> m_inputElements;
        int m_vertexSize;
        D3D11_PRIMITIVE_TOPOLOGY m_primitiveTopology;

        Microsoft::WRL::ComPtr<ID3D11Buffer>        m_vertexBuffer;
        Microsoft::WRL::ComPtr<ID3D11Buffer>        m_indexBuffer;

        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_vertexSRV;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_indexSRV;

        int m_vertexCount;
        int m_primitiveCount;
        int m_indexCount;

        int m_baseVertex; //first vertex of the mesh (as offset for bonesID)
    };
}