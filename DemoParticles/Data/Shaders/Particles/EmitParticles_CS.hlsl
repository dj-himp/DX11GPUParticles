
#include "ParticlesGlobals.h"

cbuffer emitterConstantBuffer : register(b0)
{
    float4 emitterPosition;
    float4 emitterDirection;
    uint emitterMaxSpawn;

    uint3 emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);

//spawn per batch of 1024 particles

[numthreads(1024, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        Particle p = (Particle) 0;
        
        p.position = emitterPosition;

        //test
        p.position += (id.x / 1024.0) * emitterDirection;
        p.age = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
