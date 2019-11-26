#include "../Globals.h"
#include "ParticlesGlobals.h"

cbuffer emitterConstantBuffer : register(b4)
{
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
RWStructuredBuffer<BakedParticle> bakedParticle : register(u2);


[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        Particle p = (Particle) 0;
        BakedParticle bp = bakedParticle[id.x];

        p.position = bp.position;

        //p.velocity = bp.normal * 0.01f;
        //useless at the moment
        p.velocity = particlesBaseSpeed * float4(0.0, 0.0, 0.0, 1.0);
        
        p.normal = bp.normal;

        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.color = color;
        
        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
