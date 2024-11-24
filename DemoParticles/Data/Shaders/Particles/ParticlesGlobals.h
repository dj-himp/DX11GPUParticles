
struct Particle
{
    float4 position;
    float4 normal;
    float4 velocity;
    float4 color;
    float4 uvSprite;

    float lifeSpan;
    float age;
    float mass;
    uint orientation;
    float sizeStart;
    float sizeEnd;

};

struct ParticleIndexElement
{
    float distance;
    float index;
};

struct VertexObject
{
    float3 position;
    float4   color;
    float3 normal;
    float3 tangent;
    float3 bitangent;
    float2 uv;
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