
#include "ParticlesGlobals.h"

cbuffer emitterConstantBuffer : register(b1)
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

        //p.velocity = emitterDirection;
        p.velocity = float4(0.0, 0.0, 0.0, 0.0);
        
        p.lifeSpan = 2.0;
        p.age = p.lifeSpan;
        p.mass = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
