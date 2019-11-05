
#include "../Globals.h"
#include "ParticlesGlobals.h"
#include "../Noises/Random.h"

cbuffer emitterConstantBuffer : register(b1)
{
    float4 emitterPosition;
    float4 emitterDirection;
    uint emitterMaxSpawn;

    uint3 emitterPadding;
};

ConsumeStructuredBuffer<uint> deadListBuffer : register(u0);
RWStructuredBuffer<Particle> particleList : register(u1);


float3 pointOnSphere(float3 center, float radius)
{
    float3 p;
    float norm;
    float radiusDouble = 2.0 * radius;
    do
    {
        p = float3(radiusDouble * rand_xorshift_normalized() - radius, radiusDouble * rand_xorshift_normalized() - radius, radiusDouble * rand_xorshift_normalized() - radius);
        norm = p.x * p.x + p.y * p.y + p.z * p.z;
    } while (norm > (radius * radius));

    return p / sqrt(norm);
}


//spawn per batch of 1024 particles
[numthreads(1024, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if(id.x < nbDeadParticles && id.x < emitterMaxSpawn)
    {
        rng_state = wang_hash(id.x + (uint)time);

        Particle p = (Particle) 0;
        
        p.position = emitterPosition;
        //p.position.xyz = pointOnSphere(emitterPosition.xyz, 1.0);
        float3 position = float3(rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() - 0.5, rand_xorshift_normalized() -0.5);
        position = normalize(position);
        p.position.xyz += position;
        p.position.w = 1.0;

        //p.velocity = emitterDirection;
        p.velocity = float4(0.0, 0.0, 0.0, 0.0);
        //p.velocity = float4(rand_xorshift_normalized(), rand_xorshift_normalized(), rand_xorshift_normalized(), 0.0);
        //p.velocity = p.velocity * 2.0 - 1.0;
        
        p.lifeSpan = 1.0;// * rand_xorshift_normalized();
        p.age = p.lifeSpan;
        p.mass = 1.0;

        uint index = deadListBuffer.Consume();
        particleList[index] = p;

    }

}
