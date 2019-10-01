#include "pch.h"
#include "AxisCreator.h"

#include "../Common/DirectXHelper.h"
#include "../Content/ShaderStructures.h"
#include "Model.h"
#include "ModelMesh.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    AxisCreator::AxisCreator(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : m_deviceResources(deviceResources)
    {
    }


    AxisCreator::~AxisCreator()
    {
    }

    std::unique_ptr<Model> AxisCreator::create()
    {
        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);
        std::unique_ptr<ModelMesh>& mesh = model->AddMesh();

        static const VertexColor vertices[] =
        {
            //X axis
           { Vector3(0.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f) },
           { Vector3(1.0f, 0.0f, 0.0f), Color(1.0f, 0.0f, 0.0f) },

           //Y axis
           { Vector3(0.0f, 0.0f, 0.0f), Color(0.0f, 1.0f, 0.0f) },
           { Vector3(0.0f, 1.0f, 0.0f), Color(0.0f, 1.0f, 0.0f) },

           //Z axis
           { Vector3(0.0f, 0.0f, 0.0f), Color(0.0f, 0.0f, 1.0f) },
           { Vector3(0.0f, 0.0f, 1.0f), Color(0.0f, 0.0f, 1.0f) },
           
        };

        int vertexCount = ARRAYSIZE(vertices);

        std::vector<D3D11_INPUT_ELEMENT_DESC> vertexElements;

        vertexElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        UINT vertexSize = sizeof(Vector3);

        vertexElements.push_back({ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        vertexSize += sizeof(Color);

        model->setInputElements(vertexElements);

        mesh->setInputElements(vertexElements);
        mesh->setVertexSize(vertexSize);
        mesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth = sizeof(VertexColor) * vertexCount;
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
}
