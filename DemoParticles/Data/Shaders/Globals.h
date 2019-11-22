

cbuffer sceneConstantBuffer : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;

    float4 camPosition;
    float4 camDirection;

    float4 FrustumCorners[4];

    float4 sunDirection;
    float4 sunColor; // w is diffuse power
    float4 sunSpecColor; // w is specPower
    float sceneAmbientPower;

    float time;
    float dt;
    float rngSeed;

    //float padding;
};

struct BakedParticle
{
    float4 position;
    float4 normal;
};
