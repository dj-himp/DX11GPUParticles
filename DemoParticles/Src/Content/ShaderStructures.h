#pragma once

namespace DemoParticles
{
    __declspec(align(16))
    struct SceneConstantBuffer
    {
        DirectX::SimpleMath::Matrix view;
        DirectX::SimpleMath::Matrix projection;
        DirectX::SimpleMath::Matrix viewProj;

        DirectX::XMFLOAT4 camPosition;
        DirectX::XMFLOAT4 camDirection;
        float             time;
        float             dt;

        DirectX::XMFLOAT4 frustumCorner[4];
    };

    /*struct ModelViewProjectionConstantBuffer
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Matrix view;
        DirectX::SimpleMath::Matrix projection;
    };*/

    struct QuadConstantBuffer
    {
        DirectX::SimpleMath::Matrix posScale;
    };

    struct WorldConstantBuffer
    {
        DirectX::SimpleMath::Matrix world;
    };
    /*
    struct ModelConstantBuffer
    {
        DirectX::SimpleMath::Matrix worldViewProj;
    };

    struct FrustrumCornersConstantBuffer
    {
        DirectX::XMFLOAT4 frustumCorner[4];
    };

    __declspec(align(16))
    struct MengerPSConstantBuffer
    {
        DirectX::XMFLOAT4 camPosition;
        DirectX::XMFLOAT4 camDirection;
        float             time;
    };*/

    struct EmitterConstantBuffer
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector4 direction;
        UINT maxSpawn;

        UINT padding[3];

    };

    struct DeadListCountConstantBuffer
    {
        UINT nbDeadParticles;

        UINT padding[3];
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

    struct VertexColorUV
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Color   color;
        DirectX::SimpleMath::Vector2 UV;
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