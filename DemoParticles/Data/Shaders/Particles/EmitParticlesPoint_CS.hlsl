
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterConstantBuffer : register(b4)
{
    float4 emitterPosition;
    float4 color;
    
    uint emitterMaxSpawn;
    uint particleOrientation;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesMass;
    
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
        rng_state = wang_hash(id.x + rngSeed);
        
        Particle p = (Particle) 0;
        
        p.position = emitterPosition;
        p.position.w = 1.0;

        //p.velocity = float4(0.0, 0.0, 0.0, 0.0);
        p.velocity = particlesBaseSpeed * normalize(float4(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, 0.0)) * rand_xorshift_normalized();
        
        p.lifeSpan = particlesLifeSpan * rand_xorshift_normalized();
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.color = color;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}