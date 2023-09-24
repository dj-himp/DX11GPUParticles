#include "Globals.h"

RWBuffer<uint> indirectComputeArgs : register(u0);

[numthreads(1, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    indirectComputeArgs[0] = 0; 
    indirectComputeArgs[1] = 1;
    indirectComputeArgs[2] = 1;    

}
