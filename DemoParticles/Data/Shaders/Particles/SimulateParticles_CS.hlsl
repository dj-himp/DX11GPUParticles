
#include "ParticlesGlobals.h"


RWBuffer<uint> indirectDrawArgs : register(u0);

//256 particles per thread group
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    //first thread to initialise the arguments
    if(id.x == 1)
    {
        indirectDrawArgs[0] = 0; // Number of primitives reset to zero
        indirectDrawArgs[1] = 1; // Number of instances is always 1
        indirectDrawArgs[2] = 0;
        indirectDrawArgs[3] = 0;
        indirectDrawArgs[4] = 0;
    }

    InterlockedAdd(indirectDrawArgs[0], 1);

}
