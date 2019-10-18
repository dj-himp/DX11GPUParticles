
#include "../Globals.h"
#include "ParticlesGlobals.h"


RWBuffer<uint> indirectDrawArgs : register(u0);
RWStructuredBuffer<ParticleIndexElement> aliveParticleIndex: register(u1);
AppendStructuredBuffer<uint> deadParticleIndex : register(u2);
RWStructuredBuffer<Particle> particleList : register(u3);

//256 particles per thread group
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
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

    Particle p = particleList[id.x];
    if(p.age > 0)
    {
        
        //lifeSpan < 0.0 => invulnerable
        if(p.lifeSpan >= 0.0)
        {
            p.age -= dt;
            p.position += p.velocity * dt;
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
