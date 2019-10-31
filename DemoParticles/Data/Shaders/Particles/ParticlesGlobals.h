
struct Particle
{
    float4 position;
    float4 normal;
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

cbuffer renderParticlesGlobals : register(b1)
{
    uint orientation;

    uint3 particlesGlobalPadding;
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

static uint FORCEFIELD_TYPE_POINT = 0;
static uint FORCEFIELD_TYPE_PLANE = 1;
static uint FORCEFIELD_TYPE_CUSTOM = 1;

static uint PARTICLE_ORIENTATION_BILLBOARD = 0;
static uint PARTICLE_ORIENTATION_BACKED_NORMAL = 1;
static uint PARTICLE_ORIENTATION_DIRECTION = 2;