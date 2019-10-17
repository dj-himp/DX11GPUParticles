#include "../Globals.h"
#include "ParticlesGlobals.h"

cbuffer emitterConstantBuffer : register(b1)
{
    uint emitterMaxSpawn;

    uint3 emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
RWStructuredBuffer<BakedParticle> bakedParticle : register(u2);


//spawn per batch of 1024 particles

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        Particle p = (Particle) 0;
        BakedParticle bp = bakedParticle[id.x];

        p.position = bp.position;

        p.velocity = bp.normal * 0.1f;// float4(0.0, 0.0, 0.0, 1.0);
        
        p.lifeSpan = 5.0;
        p.age = p.lifeSpan;
        p.mass = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
