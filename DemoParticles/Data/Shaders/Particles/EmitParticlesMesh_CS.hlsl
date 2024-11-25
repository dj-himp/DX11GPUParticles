
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterMeshConstantBuffer : register(b4)
{
    float4x4 world;
    float4 colorStart;
    float4 colorEnd;
    float4 uvSprite;

    uint emitterMaxSpawn;
    uint particleOrientation;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesMass;
    float particleSizeStart;
    float particleSizeEnd;
    
    uint emitterMeshPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex :register(u2);
RWBuffer<uint> indirectDispatchArgs : register(u3);

StructuredBuffer<VertexObject> meshVertices : register(t0);
StructuredBuffer<int> meshindexes : register(t1);


// Function to generate a random point within a triangle
float3 randomPointInTriangle(float3 v0, float3 v1, float3 v2) {
    float r1 = rand_xorshift_normalized();
    float r2 = rand_xorshift_normalized();
    float sqrtR1 = sqrt(r1);

    return v0 + (v1 - v0) * sqrtR1 + (v2 - v0) * r2;
}

//spawn per batch of 256
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        rng_state = wang_hash(id.x + time);
        //rng_state = wang_hash(id.x + rng_state);

        Particle p = (Particle)0;

        p.uvSprite = uvSprite;

        //uint nbVertices;
        //uint stride;
        //meshVertices.GetDimensions(nbVertices, stride);

        //uint id = uint(rand_xorshift()) % nbVertices;
        //p.position = float4(meshVertices[id].position, 1.0);

        uint nbIndexes;
        uint stride;
        meshindexes.GetDimensions(nbIndexes, stride);

        uint triangleIndex = uint(rand_xorshift()) % (nbIndexes / 3);
        //uint triangleIndex = rand_xorshift_normalized() * (nbIndexes / 3);
        uint index0 = meshindexes[triangleIndex * 3];
        uint index1 = meshindexes[triangleIndex * 3 + 1];
        uint index2 = meshindexes[triangleIndex * 3 + 2];

        float3 v0 = meshVertices[index0].position;
        float3 v1 = meshVertices[index1].position;
        float3 v2 = meshVertices[index2].position;

        float3 randomPoint = randomPointInTriangle(v0, v1, v2);
        p.position = float4(randomPoint, 1.0);

        p.position = mul(p.position, world);

        //useless for the moment
        p.velocity = float4(particlesBaseSpeed * meshVertices[index0].normal, 0.0);
        //p.velocity = float4(rand_xorshift_normalized(), rand_xorshift_normalized(), rand_xorshift_normalized(), 0.0);
        
        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.colorStart = colorStart;
        p.colorEnd = colorEnd;
        p.sizeStart = particleSizeStart;
        p.sizeEnd = particleSizeEnd;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

        ParticleIndexElement pe;
        pe.index = index;
        pe.distance = 0; //initialized in simulation
        aliveParticleIndex.Append(pe);

        InterlockedAdd(indirectDispatchArgs[0], 1);
    }

}

//USEFULL FOR ATTRACTOR
/*float3 calculateBarycentric(float3 p, float3 a, float3 b, float3 c) {
    float3 v0 = b - a, v1 = c - a, v2 = p - a;
    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);
    float denom = d00 * d11 - d01 * d01;
    float3 barycentric;
    barycentric.y = (d11 * d20 - d01 * d21) / denom;
    barycentric.z = (d00 * d21 - d01 * d20) / denom;
    barycentric.x = 1.0 - barycentric.y - barycentric.z;
    return barycentric;
}*/