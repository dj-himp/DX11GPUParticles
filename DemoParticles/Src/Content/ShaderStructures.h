#pragma once

namespace DemoParticles
{
    //__declspec(align(16))
    struct SceneConstantBuffer
    {
        DirectX::SimpleMath::Matrix view;
        DirectX::SimpleMath::Matrix projection;
        DirectX::SimpleMath::Matrix viewProj;

        DirectX::SimpleMath::Vector4 camPosition;
        DirectX::SimpleMath::Vector4 camDirection;

        DirectX::SimpleMath::Vector4 frustumCorner[4];

        DirectX::SimpleMath::Vector4 sunDirection;
        DirectX::SimpleMath::Vector4 sunColor; // w is diffuse power
        DirectX::SimpleMath::Vector4 sunSpecColor; // w is specPower
        float sceneAmbientPower;

        //put non float4 at the end
        float             time;
        float             dt;
        float             rngSeed;

        //UINT padding[1];
    };

    struct QuadConstantBuffer
    {
        DirectX::SimpleMath::Matrix posScale;
    };

    struct WorldConstantBuffer
    {
        DirectX::SimpleMath::Matrix world;
    };
    
    struct EmitterSphereConstantBuffer
    {
        DirectX::SimpleMath::Vector4 position;
        UINT maxSpawn;

        UINT padding[3];

    };

    struct EmitterCubeConstantBuffer
    {
        DirectX::SimpleMath::Matrix world;
        UINT maxSpawn;

        UINT padding[3];

    };

    struct EmitterFromBufferConstantBuffer
    {
        UINT maxSpawn;

        UINT padding[3];

    };

    struct DeadListCountConstantBuffer
    {
        UINT nbDeadParticles;

        UINT padding[3];
    };

    struct InitIndirectComputeArgs1DConstantBuffer
    {
        UINT nbThreadGroupX;

        UINT padding[3];
    };

    struct ParticlesGlobalsConstantBuffer
    {
        DirectX::SimpleMath::Vector4 color;
        UINT particleOrientation;
        UINT addForceField;
        UINT addAizama;
        UINT addCurlNoise;
        UINT addDrag;

        UINT padding[3];
    };

    struct SimulateParticlesConstantBuffer
    {
        //TEMP
        DirectX::SimpleMath::Matrix forceFieldWorld2Volume;
        DirectX::SimpleMath::Matrix forceFieldVolume2World;

        UINT nbWantedAttractors;

        UINT padding[3];
    };

    struct RenderForceFieldConstantBuffer
    {
        DirectX::SimpleMath::Vector3 size;

        UINT padding[1];
    };

#define MAX_ATTRACTORS 4 //also change it in the shader
    struct Attractor
    {
        DirectX::SimpleMath::Vector4 position;
        float gravity;
        float mass;
        float killZoneRadius;

        UINT padding[1];
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