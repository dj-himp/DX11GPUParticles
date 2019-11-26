
struct Particle
{
    float4 position;
    float4 normal;
    float4 velocity;
    float4 color;

    float lifeSpan;
    float age;
    float mass;
    uint orientation;

};

struct ParticleIndexElement
{
    float distance;
    float index;
};

cbuffer renderParticlesGlobals : register(b1)
{
    //float4 color;

    uint4 particlesGlobalPadding;
}

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

static uint PARTICLE_ORIENTATION_BILLBOARD = 0;
static uint PARTICLE_ORIENTATION_BACKED_NORMAL = 1;
static uint PARTICLE_ORIENTATION_DIRECTION = 2;