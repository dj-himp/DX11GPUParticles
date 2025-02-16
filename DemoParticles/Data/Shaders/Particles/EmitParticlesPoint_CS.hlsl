
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterConstantBuffer : register(b4)
{
    float4x4 emitterRotation;
    float4 emitterPosition;
    float4 colorStart;
    float4 colorEnd;
    float4 uvSprite;
    
    uint emitterMaxSpawn;
    float emitterConeColatitude;
    float emitterConeLongitude;
    uint particleOrientation;
    float particlesBaseSpeed;
    float particlesLifeSpan;
    float particlesMass;
    float particleSizeStart;
    float particleSizeEnd;
    
    uint3 emitterPadding;
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
        rng_state = wang_hash(id.x + rngSeed);
        
        Particle p = (Particle) 0;
        
        p.position = emitterPosition;
        p.position.w = 1.0;

        p.uvSprite = uvSprite;

        float colatitude = (0.5 - rand_xorshift_normalized()) * emitterConeColatitude;
        float longitude = (0.5 - rand_xorshift_normalized()) * emitterConeLongitude;
        
        float radius = 1.0f;
        p.velocity.x = radius * sin(colatitude) * cos(longitude);
        p.velocity.z = radius * sin(colatitude) * sin(longitude);
        p.velocity.y = radius * cos(colatitude);
        p.velocity.w = 1.0f;
        
        p.velocity = mul(p.velocity, emitterRotation);
        
        p.velocity = particlesBaseSpeed * normalize(p.velocity);
        
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
