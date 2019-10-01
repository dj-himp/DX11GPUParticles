#include "pch.h"
#include "QuadCreator.h"

#include "../Common/DirectXHelper.h"
#include "../Content/ShaderStructures.h"
#include "Model.h"
#include "ModelMesh.h"

using namespace DirectX::SimpleMath;

namespace DemoParticles
{
    QuadCreator::QuadCreator(const std::shared_ptr<DX::DeviceResources>& deviceResources)
        : m_deviceResources(deviceResources)
    {
    }


    QuadCreator::~QuadCreator()
    {
    }

    std::unique_ptr<Model> QuadCreator::create()
    {
        std::unique_ptr<Model> model = std::make_unique<Model>(m_deviceResources);
        std::unique_ptr<ModelMesh>& mesh = model->AddMesh();

        static const VertexPositionUV vertices[] =
        {
           { Vector4(-1.0f, +1.0f, 0.0f, 1.0f), Vector2(0.0f, 0.0f) },
           { Vector4(+1.0f, +1.0f, 0.0f, 1.0f), Vector2(1.0f, 0.0f) },
           { Vector4(-1.0f, -1.0f, 0.0f, 1.0f), Vector2(0.0f, 1.0f) },
           { Vector4(+1.0f, -1.0f, 0.0f, 1.0f), Vector2(1.0f, 1.0f) },
        };

        int vertexCount = ARRAYSIZE(vertices);

        /*static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
        {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 16, D3D11_INPUT_PER_VERTEX_DATA, 0 },
        };*/

        std::vector<D3D11_INPUT_ELEMENT_DESC> vertexElements;

        vertexElements.push_back({ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        UINT vertexSize = sizeof(Vector4);

        vertexElements.push_back({ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, vertexSize, D3D11_INPUT_PER_VERTEX_DATA, 0 });
        vertexSize += sizeof(Vector2);

        model->setInputElements(vertexElements);
        
        mesh->setInputElements(vertexElements);
        mesh->setVertexSize(vertexSize);
        mesh->setPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        D3D11_BUFFER_DESC vertexBufferDesc;
        vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
        vertexBufferDesc.ByteWidth = sizeof(VertexPositionUV) * vertexCount;
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
            //0,2,3,
            //3,1,0,
            0,1,3,
            3,2,0,
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