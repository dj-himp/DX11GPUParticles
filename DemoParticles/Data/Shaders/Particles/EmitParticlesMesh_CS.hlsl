
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterCubeConstantBuffer : register(b4)
{
    float4x4 world;
    float4 color;
    
    uint emitterMaxSpawn;
    uint particleOrientation;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesMass;
    float particleSizeStart;
    float particleSizeEnd;
    
    uint emitterCubePadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
StructuredBuffer<VertexObject> meshVertices : register(t0);
StructuredBuffer<int> meshindexes : register(t1);


// Function to generate a random point within a triangle
float3 randomPointInTriangle(float3 v0, float3 v1, float3 v2) {
    float r1 = rand_xorshift_normalized();
    float r2 = rand_xorshift_normalized();
    float sqrtR1 = sqrt(r1);

    return v0 + (v1 - v0) * sqrtR1 + (v2 - v0) * r2;
}

//spawn per batch of 1024 particles
[numthreads(1024, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        rng_state = wang_hash(id.x * time);

        Particle p = (Particle)0;

        //uint nbVertices;
        //uint stride;
        //meshVertices.GetDimensions(nbVertices, stride);

        //uint id = uint(rand_xorshift()) % nbVertices;
        //p.position = float4(meshVertices[id].position, 1.0);

        uint nbIndexes;
        uint stride;
        meshindexes.GetDimensions(nbIndexes, stride);

        uint triangleIndex = uint(rand_xorshift()) % (nbIndexes / 3);
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
        p.velocity = particlesBaseSpeed * float4(0.0, 0.0, 0.0, 0.0);
        //p.velocity = float4(rand_xorshift_normalized(), rand_xorshift_normalized(), rand_xorshift_normalized(), 0.0);
        
        p.lifeSpan = particlesLifeSpan * rand_xorshift_normalized();
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.color = color;
        p.sizeStart = particleSizeStart;
        p.sizeEnd = particleSizeEnd;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

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