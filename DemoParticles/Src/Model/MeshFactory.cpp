#include "pch.h"
#include "MeshFactory.h"

#include "Content/ShaderStructures.h"
#include "Model.h"
#include "ModelMesh.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    MeshFactory::MeshFactory()
    {
        m_vertexElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        m_vertexSize = sizeof(Vector4);

        m_vertexElements.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, m_vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        m_vertexSize += sizeof(Color);

        m_vertexElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, m_vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        m_vertexSize += sizeof(Vector2);
    }

    MeshFactory& MeshFactory::getInstance()
    {
        static MeshFactory instance = MeshFactory();
        return instance;
    }

    void MeshFactory::setDeviceResources(const DX::DeviceResources* deviceResources)
    {
        m_deviceResources = deviceResources;
    }

    std::unique_ptr<Model> MeshFactory::createAxis()
    {
        if (m_deviceResources == nullptr)
        {
            assert(0);
            return nullptr;
        }

        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);
        std::unique_ptr<ModelMesh>& mesh = model->AddMesh();

        Vector2 dummyUV = Vector2(0.0f);

        static const VertexColorUV vertices[] =
        {
            //X axis
           { Vector4(0.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 0.0f, 0.0f), dummyUV},
           { Vector4(1.0f, 0.0f, 0.0f, 1.0f), Color(1.0f, 0.0f, 0.0f), dummyUV},

           //Y axis
           { Vector4(0.0f, 0.0f, 0.0f, 1.0f), Color(0.0f, 1.0f, 0.0f), dummyUV},
           { Vector4(0.0f, 1.0f, 0.0f, 1.0f), Color(0.0f, 1.0f, 0.0f), dummyUV},

           //Z axis
           { Vector4(0.0f, 0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f), dummyUV},
           { Vector4(0.0f, 0.0f, 1.0f, 1.0f), Color(0.0f, 0.0f, 1.0f), dummyUV},
           
        };

        int vertexCount = ARRAYSIZE(vertices);

        model->setInputElements(m_vertexElements);
        model->setVertexStride(sizeof(VertexColorUV));

        mesh->setInputElements(m_vertexElements);
        mesh->setVertexSize(m_vertexSize);
        mesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth = sizeof(VertexColorUV) * vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferData;
        vertexBufferData.pSysMem = &vertices[0];
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &vertexBuffer
            )
        );

        mesh->setVertexBuffer(vertexBuffer);
        mesh->setVertexCount(vertexCount);
        mesh->setPrimitiveCount(3);


        static const unsigned int indices[] =
        {
            0,1,
            2,3,
            4,5
        };

        int indexCount = ARRAYSIZE(indices);

        D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.ByteWidth = (UINT)(sizeof(int) * indexCount);
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData;
        indexBufferData.pSysMem = &indices[0];
        //indexBufferData.SysMemPitch = 0;
        //indexBufferData.SysMemSlicePitch = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &indexBuffer
            )
        );

        mesh->setIndexBuffer(indexBuffer);
        mesh->setIndexCount(indexCount);

        return std::move(model);
    }

    std::unique_ptr<Model> MeshFactory::createQuad()
    {
        if (m_deviceResources == nullptr)
        {
            assert(0);
            return nullptr;
        }

        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);
        std::unique_ptr<ModelMesh>& mesh = model->AddMesh();

        Color dummyColor = Color(0, 0, 0);

        static const VertexColorUV vertices[] =
        {
           { Vector4(-1.0f, +1.0f, 0.0f, 1.0f), Color(1,0,0), Vector2(0.0f, 0.0f) },
           { Vector4(+1.0f, +1.0f, 0.0f, 1.0f), Color(0,1,0), Vector2(1.0f, 0.0f) },
           { Vector4(-1.0f, -1.0f, 0.0f, 1.0f), Color(0,0,1), Vector2(0.0f, 1.0f) },
           { Vector4(+1.0f, -1.0f, 0.0f, 1.0f), Color(1,0,1), Vector2(1.0f, 1.0f) },
        };

        int vertexCount = ARRAYSIZE(vertices);

        model->setInputElements(m_vertexElements);
        model->setVertexStride(sizeof(VertexColorUV));

        mesh->setInputElements(m_vertexElements);
        mesh->setVertexSize(m_vertexSize);
        mesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth = sizeof(VertexColorUV) * vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferData;
        vertexBufferData.pSysMem = &vertices[0];
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &vertexBuffer
            )
        );

        mesh->setVertexBuffer(vertexBuffer);
        mesh->setVertexCount(vertexCount);
        mesh->setPrimitiveCount(2);


        static const unsigned int indices[] =
        {
            //0,1,3,
            //3,2,0,
            1,0,2,
            2,3,1,
        };

        int indexCount = ARRAYSIZE(indices);

        D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.ByteWidth = (UINT)(sizeof(int) * indexCount);
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData;
        indexBufferData.pSysMem = &indices[0];
        //indexBufferData.SysMemPitch = 0;
        //indexBufferData.SysMemSlicePitch = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &indexBuffer
            )
        );

        mesh->setIndexBuffer(indexBuffer);
        mesh->setIndexCount(indexCount);

        return std::move(model);
    }

    std::unique_ptr<DemoParticles::Model> MeshFactory::createFrustum(const std::vector<DirectX::SimpleMath::Vector3> corners)
    {
        if (m_deviceResources == nullptr)
        {
            assert(0);
            return nullptr;
        }

        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);
        std::unique_ptr<ModelMesh>& mesh = model->AddMesh();

        Color color(1.0f, 0.0f, 0.0f);
        Vector2 DummyUV(0.0f);

        std::vector<VertexColorUV> vertices;
        for (auto corner : corners)
        {
            vertices.push_back({ DX::toVector4(corner), color, DummyUV });
        }

        UINT vertexCount = (UINT)vertices.size();

        model->setInputElements(m_vertexElements);
        model->setVertexStride(sizeof(VertexColorUV));

        mesh->setInputElements(m_vertexElements);
        mesh->setVertexSize(m_vertexSize);
        mesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth = sizeof(VertexColorUV) * vertexCount;
        vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        vertexBufferDesc.CPUAccessFlags = 0;
        vertexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA vertexBufferData;
        vertexBufferData.pSysMem = &vertices[0];
        vertexBufferData.SysMemPitch = 0;
        vertexBufferData.SysMemSlicePitch = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &vertexBufferDesc,
                &vertexBufferData,
                &vertexBuffer
            )
        );

        mesh->setVertexBuffer(vertexBuffer);
        mesh->setVertexCount(vertexCount);
        


        static const unsigned int indices[] =
        {
            //near
            0,1,
            1,3,
            3,2,
            2,0,

            //far
            4,5,
            5,7,
            7,6,
            6,4,

            //sides
            0,4,
            1,5,
            2,6,
            3,7
        };

        int indexCount = ARRAYSIZE(indices);
        int primitiveCount = indexCount / 2;

        mesh->setPrimitiveCount(primitiveCount);

        D3D11_BUFFER_DESC indexBufferDesc;
        indexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        indexBufferDesc.ByteWidth = (UINT)(sizeof(int) * indexCount);
        indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
        indexBufferDesc.CPUAccessFlags = 0;
        indexBufferDesc.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA indexBufferData;
        indexBufferData.pSysMem = &indices[0];
        //indexBufferData.SysMemPitch = 0;
        //indexBufferData.SysMemSlicePitch = 0;

        Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;

        DX::ThrowIfFailed(
            m_deviceResources->GetD3DDevice()->CreateBuffer(
                &indexBufferDesc,
                &indexBufferData,
                &indexBuffer
            )
        );

        mesh->setIndexBuffer(indexBuffer);
        mesh->setIndexCount(indexCount);

        return std::move(model);
    }

}
