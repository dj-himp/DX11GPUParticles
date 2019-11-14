
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/SimplexNoise3D.h"

cbuffer simulateParticlesConstantBuffer : register(b4)
{
    //TEMP
    float4x4 forceFieldWorld2Volume;

    uint nbWantedAttractors;

    uint3 simulatePadding;
}

struct Attractor
{
    float4 position;
    float gravity;
    float mass;
    float killZoneRadius;

    uint attractorPadding;
};

struct ForceField
{

};

RWBuffer<uint> indirectDrawArgs : register(u0);
RWStructuredBuffer<ParticleIndexElement> aliveParticleIndex: register(u1);
AppendStructuredBuffer<uint> deadParticleIndex : register(u2);
RWStructuredBuffer<Particle> particleList : register(u3);

StructuredBuffer<Attractor> attractorBuffer : register(t0);
Texture2D<float4> noiseTexture : register(t1);
Texture3D<float4> forceFieldTexture : register(t2);

SamplerState LinearWrapSampler : register(s0);

#define MAX_ATTRACTORS 4
groupshared Attractor attractorList[MAX_ATTRACTORS];

float3 snoiseVec3(float3 x)
{

    float s = snoise(float3(x));
    float s1 = snoise(float3(x.y - 19.1, x.z + 33.4, x.x + 47.2));
    float s2 = snoise(float3(x.z + 74.2, x.x - 124.5, x.y + 99.4));
    float3 c = float3(s, s1, s2);
    return c;

}

float3 curlNoise(float3 p)
{
    float epsilon = 0.1;
    float3 dx = float3(epsilon, 0.0, 0.0);
    float3 dy = float3(0.0, epsilon, 0.0);
    float3 dz = float3(0.0, 0.0, epsilon);

    float3 p_x0 = snoiseVec3(p - dx);
    float3 p_x1 = snoiseVec3(p + dx);
    float3 p_y0 = snoiseVec3(p - dy);
    float3 p_y1 = snoiseVec3(p + dy);
    float3 p_z0 = snoiseVec3(p - dz);
    float3 p_z1 = snoiseVec3(p + dz);

    float x = p_y1.z - p_y0.z - p_z1.y + p_z0.y;
    float y = p_z1.x - p_z0.x - p_x1.z + p_x0.z;
    float z = p_x1.y - p_x0.y - p_y1.x + p_y0.x;

    float divisor = 1.0 / (2.0 * epsilon);

    return normalize(float3(x, y, z) * divisor);
}

//256 particles per thread group
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID, uint groupId : SV_GroupIndex) //SV_GroupIndex is a flatenned index
{
    //first thread to initialise the arguments
    if(id.x == 0)
    {
        indirectDrawArgs[0] = 0; // Number of primitives reset to zero
        indirectDrawArgs[1] = 1; // Number of instances is always 1
        indirectDrawArgs[2] = 0;
        indirectDrawArgs[3] = 0;
        indirectDrawArgs[4] = 0;
    }

    // Wait after draw args are written so no other threads can write to them before they are initialized
    GroupMemoryBarrierWithGroupSync();

    //load attractors in to shared memory
    uint nbAttractors = min(nbWantedAttractors, MAX_ATTRACTORS);
    if (groupId < nbAttractors)
    {
        attractorList[groupId].position = attractorBuffer[groupId].position;
        attractorList[groupId].gravity = attractorBuffer[groupId].gravity;
        attractorList[groupId].mass = attractorBuffer[groupId].mass;
        attractorList[groupId].killZoneRadius = attractorBuffer[groupId].killZoneRadius;
    }

    GroupMemoryBarrierWithGroupSync();

    Particle p = particleList[id.x];
    if(p.age > 0)
    {
        
        //lifeSpan < 0.0 => invulnerable
        if(p.lifeSpan >= 0.0)
        {
            p.age -= dt;
        }

        float4 particleForce = 0;

        for (uint i = 0; i < nbAttractors; ++i)
        {
            Attractor a = attractorList[i];
            float4 direction = a.position - p.position;
            float distance = length(direction);

            particleForce += normalize(direction) * (a.gravity * p.mass * a.mass) / (max(1.0, distance * distance));
        }

        float3 forceFieldUV = mul(float4(p.position.xyz, 1.0), forceFieldWorld2Volume).xyz;
        float3 force = forceFieldTexture.SampleLevel(LinearWrapSampler, forceFieldUV, 0).xyz;
        force = mul(float4(force, 0.0), forceFieldWorld2Volume).xyz;
        particleForce.xyz += force;// * 10.0;

        //TEST Aizawa attractor
        /*particleForce.x = (p.position.z - 0.8) * p.position.x - 1.5 * p.position.y;
        particleForce.y = 1.5 * p.position.x + (p.position.z - 0.8) * p.position.y;
        particleForce.z = 0.688128 + 1.102 * p.position.z - (pow(p.position.z, 3) / 3.0) - (p.position.x * p.position.x + p.position.y * p.position.y) * (1.0 + 1.122 * p.position.z) + 0.1 * p.position.z * p.position.x * p.position.x * p.position.x;
        */

        bool addCurlNoise = false;
        if (addCurlNoise)
        {
            //p.position.xyz += curlNoise(p.position.xyz) * 0.1;
            //particleForce.xyz += curlNoise(p.position.xyz) * 10.0;
            particleForce.xyz += curlNoise(particleForce.xyz) * 10.0;
        }
        
        //Add drag
        bool addDrag = false;
        if(addDrag)
        {
            float dragCoefficient = 0.0001;
            particleForce -= dragCoefficient * p.velocity;
        }

        float3 acceleration = particleForce.xyz / p.mass;
        p.velocity.xyz += acceleration * dt;
        //p.velocity.xyz = particleForce.xyz;
        p.position.xyz += p.velocity.xyz * dt;

        //kill particles inside attractors killzone (if killZoneRadius >= 0.0)
        for (uint i = 0; i < nbAttractors; ++i)
        {
            Attractor a = attractorList[i];
            float distance = length(a.position - p.position);
            if(distance < a.killZoneRadius)
            {
                p.age = 0.0;
            }
        }

        if(p.age > 0)
        {
            ParticleIndexElement particle;
            particle.distance = length(p.position - camPosition);
            particle.index = id.x;
            uint index = aliveParticleIndex.IncrementCounter();
            aliveParticleIndex[index] = particle;

            InterlockedAdd(indirectDrawArgs[0], 1);
        }
        else
        {
            p.age = -1.0;
            deadParticleIndex.Append(id.x);
        }
    }

    particleList[id.x] = p;
}
