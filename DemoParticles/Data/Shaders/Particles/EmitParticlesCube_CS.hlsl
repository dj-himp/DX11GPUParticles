
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterCubeConstantBuffer : register(b4)
{
    float4x4 world;
    uint emitterMaxSpawn;

    uint3 emitterCubePadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);

//spawn per batch of 1024 particles
[numthreads(1024, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        rng_state = wang_hash(id.x * time);

        Particle p = (Particle) 0;
        
        p.position = float4(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, 1.0);
        p.position = mul(p.position, world);

        p.velocity = float4(0.0, 0.0, 0.0, 0.0);
        //p.velocity = float4(rand_xorshift_normalized(), rand_xorshift_normalized(), rand_xorshift_normalized(), 0.0);
        
        p.lifeSpan = 10.0 * rand_xorshift_normalized();
        p.age = p.lifeSpan;
        p.mass = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
