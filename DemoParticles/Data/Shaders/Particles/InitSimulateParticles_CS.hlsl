
#include "../Globals.h"
#include "ParticlesGlobals.h"


RWBuffer<uint> indirectDrawArgs : register(u0);
AppendStructuredBuffer<uint> AliveParticleIndex : register(u1);
RWStructuredBuffer<Particle> particleList : register(u2);


//256 particles per thread group
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID, uint groupId : SV_GroupIndex) //SV_GroupIndex is a flatenned index
{
    //first thread to initialise the arguments
    if(id.x == 0)
    {
        indirectDrawArgs[0] = 0;
        indirectDrawArgs[1] = 1;
        indirectDrawArgs[2] = 1;
    }

    // Wait after draw args are written so no other threads can write to them before they are initialized
    GroupMemoryBarrierWithGroupSync();

    Particle p = particleList[id.x];
    if(p.age > 0)
    {
        AliveParticleIndex.Append(id.x);
        InterlockedAdd(indirectDrawArgs[0], 1);
    }
}
