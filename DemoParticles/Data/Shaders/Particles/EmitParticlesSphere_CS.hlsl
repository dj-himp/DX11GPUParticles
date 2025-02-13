
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterConstantBuffer : register(b4)
{
    float4 emitterPosition;
    float4 emitterScale;
    float4 emitterPartitioning; //don't know how to name that but influence repartition inside the volume (making cool effects)
    float4 colorStart;
    float4 colorEnd;
    float4 uvSprite;
    float4x4 emitterRotation;
    
    uint emitterMaxSpawn;
    uint particleOrientation;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesMass;
    float particleSizeStart;
    float particleSizeEnd;
    
    uint emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex :register(u2);
RWBuffer<uint> indirectDispatchArgs : register(u3);

//spawn per batch of 256
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        //rng_state = wang_hash(id.x + time);
        rng_state = wang_hash(id.x + rngSeed);
        
        Particle p = (Particle) 0;

        p.uvSprite = uvSprite;

        p.position = emitterPosition;
        float3 position = emitterPartitioning.xyz * float3(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5);
        position = mul(float4(emitterScale.xyz * normalize(position), 1.0), emitterRotation).xyz;
        //p.position = mul(p.position, emitterRotation);
        p.position.xyz += position;
        p.position.w = 1.0;
        
        //p.velocity = particlesBaseSpeed * normalize(float4(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, 0.0));
        p.velocity = particlesBaseSpeed * normalize(float4(position - emitterPosition.xyz, 0.0));
        
        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.colorStart = colorStart;
        p.colorEnd = colorEnd;
        p.sizeStart = particleSizeStart;
        p.sizeEnd = particleSizeEnd;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;
        
        ParticleIndexElement pe;
        pe.index = index;
        pe.distance = 0; //initialized in simulation
        aliveParticleIndex.Append(pe);

        InterlockedAdd(indirectDispatchArgs[0], 1);

    }

}
