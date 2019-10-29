
#include "../Globals.h"
#include "ParticlesGlobals.h"

cbuffer simulateParticlesConstantBuffer : register(b1)
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

#define MAX_FORCE_FIELDS 4
groupshared ForceField forceFieldsList[MAX_FORCE_FIELDS];

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
            p.position += p.velocity * dt;
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

        //integration
        //p.velocity += particleForce * dt;
        //p.position += p.velocity * dt;

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
