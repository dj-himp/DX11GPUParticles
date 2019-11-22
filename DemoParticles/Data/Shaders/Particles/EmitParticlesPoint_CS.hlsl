
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterConstantBuffer : register(b4)
{
    float4 emitterPosition;
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
        //rng_state = wang_hash(id.x + time);
        rng_state = wang_hash(id.x + rngSeed);
        
        Particle p = (Particle) 0;
        
        p.position = emitterPosition;
        p.position.w = 1.0;

        //p.velocity = float4(0.0, 0.0, 0.0, 0.0);
        p.velocity = float4(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, 0.0);
        
        p.lifeSpan = 10.0;// * rand_xorshift_normalized();
        p.age = p.lifeSpan;
        p.mass = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
