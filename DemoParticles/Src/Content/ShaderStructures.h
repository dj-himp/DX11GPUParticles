#pragma once

#include "Model/Model.h" //TO DO maybe remove this reference

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
        DirectX::SimpleMath::Vector4 scale;
        DirectX::SimpleMath::Vector4 partitioning;
        DirectX::SimpleMath::Color colorStart;
        DirectX::SimpleMath::Color colorEnd;
        DirectX::SimpleMath::Vector4 uvSprite;
        DirectX::SimpleMath::Matrix rotation;

        UINT maxSpawn;
        UINT particleOrientation;
        float particlesBaseSpeed;
        float particlesLifeSpan;
        float particlesMass;
        float particleSizeStart;
        float particleSizeEnd;

        UINT padding[1];

    };

    struct EmitterPointConstantBuffer
    {
        DirectX::SimpleMath::Matrix rotation;
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Color colorStart;
        DirectX::SimpleMath::Color colorEnd;
		DirectX::SimpleMath::Vector4 uvSprite;
		
        UINT maxSpawn;
        float coneColatitude;
        float coneLongitude;
        UINT particleOrientation;
        float particlesBaseSpeed;
        float particlesLifeSpan;
        float particlesMass;
        float particleSizeStart;
        float particleSizeEnd;

        UINT padding[3];

    };

    struct EmitterCubeConstantBuffer
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Color colorStart;
        DirectX::SimpleMath::Color colorEnd;
        DirectX::SimpleMath::Vector4 uvSprite;

        UINT maxSpawn;
        UINT particleOrientation;
        float particlesBaseSpeed;
        float particlesLifeSpan;
        float particlesMass;
        float particleSizeStart;
        float particleSizeEnd;

        UINT padding[1];

    };

    struct EmitterFromBufferConstantBuffer
    {
        DirectX::SimpleMath::Color colorStart;
        DirectX::SimpleMath::Color colorEnd;
        DirectX::SimpleMath::Vector4 uvSprite;

        UINT maxSpawn;
        UINT particleOrientation;
        float particlesBaseSpeed;
        float particlesLifeSpan;
        float particlesMass;
        float particleSizeStart;
        float particleSizeEnd;

        UINT padding[1];

    };

    struct EmitterMeshConstantBuffer
    {
        DirectX::SimpleMath::Matrix world;
        DirectX::SimpleMath::Color colorStart;
        DirectX::SimpleMath::Color colorEnd;
        DirectX::SimpleMath::Vector4 uvSprite;

        UINT maxSpawn;
        UINT particleOrientation;
        float particlesBaseSpeed;
        float particlesLifeSpan;
        float particlesMass;
        float particleSizeStart;
        float particleSizeEnd;

        UINT padding[1];

    };

    struct DeadListCountConstantBuffer
    {
        UINT nbDeadParticles;

        UINT padding[3];
    };

    struct InitIndirectComputeArgs1DConstantBuffer
    {
        float nbThreadGroupX;

        UINT padding[3];
    };

    struct ParticlesGlobalsConstantBuffer
    {
        //DirectX::SimpleMath::Vector4 color;
        
        UINT padding[4];
    };

    struct SimulateParticlesConstantBuffer
    {
        //TEMP
        DirectX::SimpleMath::Matrix forceFieldWorld2Volume;
        DirectX::SimpleMath::Matrix forceFieldVolume2World;

        DirectX::SimpleMath::Vector4 aizamaParams1;
        DirectX::SimpleMath::Vector4 aizamaParams2;
        DirectX::SimpleMath::Vector4 lorenzParams1;

        float dragCoefficient;
        float curlScale;
        float curlNoiseFactor;

        float forceFieldForceScale;
        float forceFieldIntensity;

        UINT nbAttractors;

        UINT addForceField;
        UINT addAizama;
        UINT addLorenz;
        UINT addCurlNoise;
        UINT addDrag;

        UINT padding[1];
    };

    struct RenderForceFieldConstantBuffer
    {
        DirectX::SimpleMath::Vector4 size;
        
        DirectX::SimpleMath::Matrix forceFieldVolume2World;
        
    };

    struct ModelToEmitConstantBuffer
    {
        DirectX::SimpleMath::Vector2 scaleDensity;
        DirectX::SimpleMath::Vector2 offsetDensity;
    };

    struct SkinnedConstantBuffer
    {
        DirectX::SimpleMath::Matrix boneTransforms[96];
    };

    struct Particle
    {
        DirectX::SimpleMath::Vector4 position;
        DirectX::SimpleMath::Vector4 normal;
        DirectX::SimpleMath::Vector4 velocity;
        DirectX::SimpleMath::Color color;
        DirectX::SimpleMath::Color colorStart;
        DirectX::SimpleMath::Color colorEnd;
		DirectX::SimpleMath::Vector4 uvSprite;
		
        float lifeSpan;
        float age;
        float mass;
        UINT  orientation;
        float sizeStart;
        float sizeEnd;
    };

    struct ParticleIndexElement
    {
        float distance; //squared distance from camera
        float index; //index in the particle buffer
    };

#define MAX_ATTRACTORS 10
    struct Attractor
    {
        DirectX::SimpleMath::Vector4 position;
        bool enabled;
        float gravity;
        float mass;
        float killZoneRadius;
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
        DirectX::SimpleMath::Vector4 blendWeight;
        BYTE                         boneIndices[4];
    };
}