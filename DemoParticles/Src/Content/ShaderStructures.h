#pragma once

namespace DemoParticles
{
    // Constant buffer used to send MVP matrices to the vertex shader.
    struct ModelViewProjectionConstantBuffer
    {
        DirectX::XMFLOAT4X4 model;
        DirectX::XMFLOAT4X4 view;
        DirectX::XMFLOAT4X4 projection;
    };

    struct QuadConstantBuffer
    {
        DirectX::XMFLOAT4X4 posScale;
    };

    struct ModelConstantBuffer
    {
        DirectX::XMFLOAT4X4 worldViewProj;
    };

    struct FrustrumCornersConstantBuffer
    {
        DirectX::XMFLOAT4 frustumCorner[4];
    };

    // Used to send per-vertex data to the vertex shader.
    struct VertexPositionColor
    {
        DirectX::XMFLOAT3 pos;
        DirectX::XMFLOAT3 color;
    };

    struct VertexPositionUV
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector2 UV;
    };

    struct VertexColor
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Color   color;
    };

    struct VertexObject
    {
        DirectX::SimpleMath::Vector3 position;
        DirectX::SimpleMath::Color   color;
        DirectX::SimpleMath::Vector3 normal;
        DirectX::SimpleMath::Vector3 tangent;
        DirectX::SimpleMath::Vector3 bitangent;
        DirectX::SimpleMath::Vector2 uv;
    };
}