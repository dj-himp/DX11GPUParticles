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


[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        Particle p = (Particle) 0;
        BakedParticle bp = bakedParticle[id.x];

        p.position = bp.position;

        p.velocity = bp.normal * 0.1f;
        //p.velocity = float4(0.0, 0.0, 0.0, 1.0);  
        
        p.normal = bp.normal;

        p.lifeSpan = -1.0;//5.0;
        p.age = 5.0;
        p.mass = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
