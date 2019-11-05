
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/SimplexNoise3D.h"

cbuffer simulateParticlesConstantBuffer : register(b4)
{
    uint nbWantedForceFields;

    uint3 simulatePadding;
}

struct ForceField
{
    uint type;
    float4 position;
    float gravity;
    float inverse_range;

    uint forceFieldPadding;
};

RWBuffer<uint> indirectDrawArgs : register(u0);
RWStructuredBuffer<ParticleIndexElement> aliveParticleIndex: register(u1);
AppendStructuredBuffer<uint> deadParticleIndex : register(u2);
RWStructuredBuffer<Particle> particleList : register(u3);

StructuredBuffer<ForceField> forceFieldBuffer : register(t0);
Texture2D<float4> noiseTexture : register(t1);

#define MAX_FORCE_FIELDS 4
groupshared ForceField forceFieldsList[MAX_FORCE_FIELDS];

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

    //load forceFields in to shared memory
    uint nbForcefields = min(nbWantedForceFields, MAX_FORCE_FIELDS);
    if (groupId < nbForcefields)
    {
        forceFieldsList[groupId].type = forceFieldBuffer[groupId].type;
        forceFieldsList[groupId].position = forceFieldBuffer[groupId].position;
        forceFieldsList[groupId].gravity = forceFieldBuffer[groupId].gravity;
        forceFieldsList[groupId].inverse_range = forceFieldBuffer[groupId].inverse_range;
    }

    GroupMemoryBarrierWithGroupSync();

    Particle p = particleList[id.x];
    if(p.age > 0)
    {
        
        //lifeSpan < 0.0 => invulnerable
        if(p.lifeSpan >= 0.0)
        {
            p.age -= dt;
            //p.position += p.velocity * dt;
        }

        float4 particleForce = 0;

        for (uint i = 0; i < nbForcefields; ++i)
        {
            ForceField field = forceFieldsList[i];
            float4 direction = field.position - p.position;
            float distance = 0.0;
            if (field.type == FORCEFIELD_TYPE_POINT)
            {
                distance = length(direction);
                //distance += length(noiseTexture[p.position.xy]);

            }
            else if (forceFieldsList[i].type == FORCEFIELD_TYPE_PLANE)
            {
                
            }
            else if (forceFieldsList[i].type == FORCEFIELD_TYPE_CUSTOM)
            {
                uint len = length(p.position);
                direction.x = cos(len);
                direction.y = sin(len);
                direction.z = tan(len);

                distance = length(direction);

            }

            particleForce += direction * p.mass * field.gravity * (1.0 - saturate(distance * field.inverse_range));
        }

        bool addCurlNoise = true;
        if (addCurlNoise)
        {
            //p.position.xyz += curlNoise(p.position) * 0.1;
            particleForce.xyz += curlNoise(p.position) * 10.0;
        }
        
        //Add drag
        float dragCoefficient = 0.1;
        particleForce -= dragCoefficient * p.velocity;

        float3 acceleration = particleForce.xyz * dt;
        p.position.xyz += (p.velocity.xyz + 0.5f * acceleration) * dt;
        p.velocity.xyz += acceleration;

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
