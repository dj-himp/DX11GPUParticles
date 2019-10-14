
#include "ParticlesGlobals.h"

AppendStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);

[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    deadListBuffer.Append(id.x);

    particleList[id.x] = (Particle)0;

}
