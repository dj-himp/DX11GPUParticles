
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterCubeConstantBuffer : register(b4)
{
    float4x4 world;
    float4 color;
    
    uint emitterMaxSpawn;
    uint particleOrientation;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesMass;
    float particleSizeStart;
    float particleSizeEnd;
    
    uint emitterCubePadding;
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

        //useless for the moment
        p.velocity = particlesBaseSpeed * float4(0.0, 0.0, 0.0, 0.0);
        //p.velocity = float4(rand_xorshift_normalized(), rand_xorshift_normalized(), rand_xorshift_normalized(), 0.0);
        
        p.lifeSpan = particlesLifeSpan * rand_xorshift_normalized();
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.color = color;
        p.sizeStart = particleSizeStart;
        p.sizeEnd = particleSizeEnd;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
