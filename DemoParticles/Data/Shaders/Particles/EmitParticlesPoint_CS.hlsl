
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterConstantBuffer : register(b4)
{
    float4 emitterPosition;
    float4 emitterDirection;
    float4 color;
    
    uint emitterMaxSpawn;
    float emitterConeYaw;
    float emitterConePitch;
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

        float4 direction = emitterDirection;
        //p.velocity = particlesBaseSpeed * normalize(float4(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, 0.0)) * rand_xorshift_normalized();
        float yaw = (0.5 - rand_xorshift_normalized()) * emitterConeYaw;
        float pitch = (0.5 - rand_xorshift_normalized()) * emitterConePitch;
        //p.velocity = particlesBaseSpeed * normalize(float4(-sin(yaw) * cos(pitch), sin(pitch), cos(pitch) * cos(yaw), 0.0)) * rand_xorshift_normalized();
        p.velocity.x = direction.x * cos(yaw) + direction.y * sin(pitch) * sin(yaw) + direction.z * -sin(yaw) * cos(pitch);
        p.velocity.y = direction.y * cos(pitch) + direction.z * sin(pitch);
        p.velocity.z = direction.x * sin(yaw) + direction.y * -sin(pitch) * cos(yaw) + direction.z * cos(pitch) * cos(yaw);
        
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
