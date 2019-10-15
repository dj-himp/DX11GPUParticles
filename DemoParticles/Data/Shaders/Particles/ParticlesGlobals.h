
struct Particle
{
    float4 position;
    float age;
};

struct ParticleIndexElement
{
    float distance;
    float index;
};

cbuffer deadListCountConstantBuffer : register(b2)
{
    uint nbDeadParticles;

    uint3 deadListPadding;
};

cbuffer aliveListCountConstantBuffer : register(b3)
{
    uint nbAliveParticles;

    uint3 aliveListPadding;
};
