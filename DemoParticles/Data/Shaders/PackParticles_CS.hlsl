#include "Globals.h"

RWStructuredBuffer<BakedParticle> particleList : register(u0);
RWBuffer<uint> indirectComputeArgs : register(u1);

Texture2D<float4> srcTexture : register(t0);
Texture2D<float4> srcTexture2 : register(t1);

[numthreads(32, 32, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    //initialise indirectArgs
    if(id.x == 0 && id.y == 0)
    {
        indirectComputeArgs[0] = 0; //threadGoupX
        indirectComputeArgs[1] = 1; //threadGoupY
        indirectComputeArgs[2] = 1; //threadGoupZ
    }

    // Wait after draw args are written so no other threads can write to them before they are initialized
    GroupMemoryBarrierWithGroupSync();

    float4 srcPixel = srcTexture[id.xy].rgba;
    float4 srcPixel2 = srcTexture2[id.xy].rgba;
    if (srcPixel.a > 0.0)
    {
        uint oldValue = particleList.IncrementCounter();

        BakedParticle particle;
        particle.position = srcPixel;
        particle.normal = srcPixel2;

        particleList[oldValue] = particle;

        //add one threadGroup for indirectCompute packing
        InterlockedAdd(indirectComputeArgs[0], 1);

        if(srcPixel2.w > 0.5)
        {
            for (int i = 0; i < 4; ++i)
            {
                uint oldValue = particleList.IncrementCounter();

                BakedParticle particle;
                particle.position = srcPixel + i * srcPixel2 * 5.0;
                particle.normal = float4(0.0, 1.0, 0.0, 1.0);//srcPixel2;

                particleList[oldValue] = particle;

                //add one threadGroup for indirectCompute packing
                InterlockedAdd(indirectComputeArgs[0], 1);
            }

        }
    }
}

/*RWTexture2D<float4> texOutput : register(u0);
RWTexture2D<float4> texOutput2 : register(u1);
RWStructuredBuffer<uint> counterBuffer : register(u2);

Texture2D<float4> srcTexture : register(t0);
Texture2D<float4> srcTexture2 : register(t1);

[numthreads(32, 32, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    float4 srcPixel = srcTexture[id.xy].rgba;
    float4 srcPixel2 = srcTexture2[id.xy].rgba;
    if (srcPixel.a > 0.0)
    {
        uint oldValue = counterBuffer.IncrementCounter();

        uint y = oldValue / 1024;
        uint x = oldValue % 1024;
        uint2 idx = uint2(x, y);

        texOutput[idx] = srcPixel;
        texOutput2[idx] = srcPixel2;
    }
}*/
