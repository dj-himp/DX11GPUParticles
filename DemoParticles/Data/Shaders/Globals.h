

cbuffer sceneConstantBuffer : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;

    float4 camPosition;
    float4 camDirection;
    float time;
    float dt;

    float4 FrustumCorners[4];

    float2 padding;
};

struct BakedParticle
{
    float4 position;
    float4 normal;
};
