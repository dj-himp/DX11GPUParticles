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
    float particleSizeStart;
    float particleSizeEnd;
    
    uint emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);
ConsumeStructuredBuffer<Particle> particlesToAppend : register(u4);
AppendStructuredBuffer<ParticleIndexElement> aliveParticleIndex :register(u2);
RWBuffer<uint> indirectDispatchArgs : register(u3);

//spawn per batch of 256
[numthreads(256, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        Particle p = particlesToAppend.Consume();

        //p.velocity = bp.normal * 0.01f;
        //useless at the moment
        //p.velocity = particlesBaseSpeed * float4(0.0, 0.0, 0.0, 1.0);
        p.velocity = particlesBaseSpeed * p.normal;

        p.lifeSpan = particlesLifeSpan;
        p.age = abs(p.lifeSpan); //abs() so if lifetime is infinite ( < 0.0) it's still has a life
        p.mass = particlesMass;

        p.orientation = particleOrientation;
        p.color = color;
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
