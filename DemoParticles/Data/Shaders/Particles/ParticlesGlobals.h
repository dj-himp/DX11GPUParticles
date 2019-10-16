
struct Particle
{
    float4 position;
    float4 velocity;

    float lifeSpan;
    float age;
    float mass;
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
